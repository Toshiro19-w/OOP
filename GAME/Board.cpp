#include <iostream>
#include <string>
#include <algorithm>  
#include <random> 
#include "Player.h"
#include "Board.h"
#include "Constants.h"
#include "Game.h"

Board::Board(Game* gameInstance) : gameInstance(gameInstance) {}

Tile Board::createTile(const std::string& name, int cost, TileType type, int houseMax, bool owned, std::function<void(Player&)> onLand, int x, int y) {
    return Tile(name, cost, 0, type, houseMax, owned, onLand, x, y);
}

void Board::setupTilePositions() {
    int x = 0, y = 0;
    for (int i = 0; i < NUM_TILES; ++i) {
        if (i >= 0 && i < 10) {            // Đường đi từ trái sang phải (dưới cùng)
            x = i;
            y = 0;
        }
        else if (i >= 10 && i < 20) {   // Đường đi từ dưới lên trên (phải)
            x = 9;
            y = i - 10;
        }
        else if (i >= 20 && i < 30) {   // Đường đi từ phải sang trái (trên cùng)
            x = 29 - i;
            y = 9;
        }
        else if (i >= 30 && i < 40) {   // Đường đi từ trên xuống dưới (trái)
            x = 0;
            y = 39 - i;
        }
        // Cập nhật vị trí của ô đất
        board[i].x = x;
        board[i].y = y;
    }
}

void Board::handleChanceEvent(Player& player) {
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<> dis(0, gameInstance->chanceEvents.size() - 1);
    int eventIndex = dis(gen);
    gameInstance->chanceEvents[eventIndex](player);  // Execute random Chance event
}

void Board::handleComChestEvent(Player& player) {
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<> dis(0, gameInstance->communityChestEvents.size() - 1);
    int eventIndex = dis(gen);
    gameInstance->communityChestEvents[eventIndex](player);  // Execute random Community Chest event
}

void Board::sellHouse(Player& player, Tile& tile) {
    if (/*tile.owner == &player && */ tile.numHouses > 0) {
        int sellPrice = tile.housePrice / 2;  // Bán với giá 50% giá trị mua
        player.money += sellPrice;
        tile.numHouses--;  // Giảm số nhà trên ô
        std::cout << player.name << " sold a house on " << tile.name << " for $" << sellPrice << std::endl;
    }
    else {
        std::cout << "You cannot sell a house here." << std::endl;
    }
}

void Board::applyTax(Player& player, Tile& tile) {
    int taxAmount = 0;
    if (tile.type == TileType::INCOME_TAX) {
        taxAmount = player.money * 0.1;  // Thuế 10% trên số tiền hiện tại của người chơi
    }
    else if (tile.type == TileType::LUXURY_TAX) {
        taxAmount = 100;  // Thuế cố định 200 đô
    }
    player.money -= taxAmount;
    std::cout << player.name << " paid $" << taxAmount << " in taxes at " << tile.name << std::endl;
}

void Board::sendPlayerToJail(Player& player) {
    player.position = JAIL_POSITION;  // Vị trí của ô Jail
    player.isInJail = true;
    player.turnsInJail = 3;

    // Thêm người chơi vào ô Jail
    board[JAIL_POSITION].playersOnTile.push_back(&player);
    std::cout << player.name << " is sent to Jail!" << std::endl;
}

void Board::setupSpecialTiles() {
    for (int i = 0; i < NUM_TILES; ++i) {
        if (i == 0) {
            board[i] = createTile("Start", 0, TileType::START, 0, false, [](Player& player) {
                player.money += 200;
                }, 0, 0);
        }
        else if (i == 10) {
            board[i] = createTile("Jail", 0, TileType::JAIL, 0, false, [](Player& player) {
                if (!player.isInJail) {
                    std::cout << player.name << " just visiting jail." << std::endl;
                }
                }, 0, 0);
        }
        else if (i == 20) {
            board[i] = createTile("Free Parking", 0, TileType::FREE_PARKING, 0, false, nullptr, 0, 0);
        }
        else if (i == 30) {
            board[i] = createTile("Go to Jail", 0, TileType::GO_TO_JAIL, 0, false, [this](Player& player) {
                if (player.state != PlayerState::InJail) {
                    std::cout << "--------------------------------" << std::endl;
                    this->sendPlayerToJail(player);
                }
                }, 0, 0);
        }
        else if (i == 7 || i == 22 || i == 36) {
            board[i] = createTile("Chance", 0, TileType::CHANCE, 0, false, [this](Player& player) {
                this->handleChanceEvent(player);
                }, 0, 0);
        }
        else if (i == 2 || i == 17 || i == 33) {
            board[i] = createTile("ComChest", 0, TileType::COMMUNITY_CHEST, 0, false, [this](Player& player) {
                this->handleComChestEvent(player);
                }, 0, 0);
        }
        else if (i == 5 || i == 15 || i == 25 || i == 35) {
            board[i] = createTile("Beach", 200, TileType::BEACH, 1, false, nullptr, 0, 0);
        }
        else if (i == 4 || i == 12) {
            board[i] = createTile("Income Tax", 0, TileType::COMMUNITY_CHEST, 0, false, [this](Player& player) {
                Tile& tile = board[player.position];
                if (tile.type == TileType::INCOME_TAX) {
                    applyTax(player, tile);
                }
                }, 0, 0);
        }
        else if (i == 27 || i == 38) {
            board[i] = createTile("Luxury Tax", 0, TileType::COMMUNITY_CHEST, 0, false, [this](Player& player) {
                Tile& tile = board[player.position];
                if (tile.type == TileType::LUXURY_TAX) {
                    applyTax(player, tile);
                }
                }, 0, 0);
        }
        else {
            int propertyCost = 100 + (i % 10) * 20;
            board[i] = createTile("Tile " + std::to_string(i), propertyCost, TileType::NORMAL, 4, false, nullptr, 0, 0);
        }
    }
}

void Board::buyHouse(Player& player, Tile& tile) {
    // Kiểm tra nếu người chơi hiện tại là người đang thực hiện hành động
    if (&player != &gameInstance->players[gameInstance->currentPlayerIndex]) {
        std::cout << "It's not your turn to buy a house." << std::endl;
        return;
    }

    // Nếu người chơi chưa sở hữu ô đất, thêm nó vào danh sách tài sản
    if (std::find(player.properties.begin(), player.properties.end(), &tile) == player.properties.end()) {
        player.addProperty(&tile);
    }

    // Kiểm tra loại ô đất và số nhà có thể xây
    if (tile.type != TileType::NORMAL) {
        std::cout << "You can only buy a house on regular plots." << std::endl;
        return;
    }

    if (player.canBuyHouse(tile) && player.money >= tile.housePrice) {
        player.money -= tile.housePrice;
        tile.numHouses++;
        std::cout << "You have successfully purchased a house on " << tile.name << "." << std::endl;
    }
    else {
        if (tile.numHouses >= tile.maxHouses) {
            std::cout << "You have reached the limit on the number of houses on this plot." << std::endl;
        }
        else if (player.money < tile.housePrice) {
            std::cout << "You can't afford to buy a house on this plot." << std::endl;
        }
        else {
            std::cout << "You cannot buy a house on this plot." << std::endl;
        }
    }
}

void Board::buyBeach(Player& player, Tile& tile) {
    // Kiểm tra nếu người chơi hiện tại đang đứng trên bãi biển và nó chưa được sở hữu
    if (tile.type == TileType::BEACH && !tile.isOwned) {
        if (player.money >= tile.housePrice) {
            player.money -= tile.housePrice;
            tile.isOwned = true;
            player.addProperty(&tile); // Thêm bãi biển vào danh sách tài sản của người chơi
            std::cout << player.name << " bought the beach: " << std::endl;

            // Kiểm tra nếu người chơi sở hữu cả 4 bãi biển
            int beachCount = std::count_if(player.properties.begin(), player.properties.end(), [](const Tile* property) {
                return property->type == TileType::BEACH;
                });
            if (beachCount == 4) {
                std::cout << player.name << " owns all 4 beaches and wins the game!" << std::endl;
                exit(0); // Kết thúc trò chơi
            }
        }
        else {
            std::cout << player.name << " doesn't have enough money to buy the beach!" << std::endl;
        }
    }
    else {
        std::cout << "This beach is already owned or it's not a beach." << std::endl;
    }
}

void Board::renderPlayerAt(Player* player, int x, int y) {
    SDL_Texture* playerTexture = player->getSprite();
    gameInstance->drawPlayer(playerTexture, player->getX(), player->getY());
}

void Board::renderPlayers() {
    for (Player& player : gameInstance->players) {
        // Lấy vị trí hiện tại của người chơi
        Tile& tile = board[player.position];

        // Lấy số lượng người chơi trên cùng một ô (tile)
        int numPlayers = tile.playersOnTile.size();

        // Nếu không có người chơi nào trên ô, bỏ qua việc render
        if (numPlayers == 0) continue;

        // Render từng người chơi trên ô đó
        for (int i = 0; i < numPlayers; ++i) {
            Player* p = tile.playersOnTile[i];

            // Tính toán offset để tránh người chơi bị chồng lên nhau nếu có nhiều người ở cùng ô
            int offsetX = (numPlayers > 1) ? i * 10 : 0;
            int offsetY = (numPlayers > 1) ? i * 10 : 0;

            // Render người chơi với tọa độ đã điều chỉnh
            renderPlayerAt(p, tile.x + offsetX, tile.y + offsetY);
        }
    }
}

void Board::renderBoards() {
    for (int i = 0; i < NUM_TILES; ++i) {
        int x = 0, y = 0;

        // Calculate position (x, y) for each tile
        if (i < 10) {  // Top row
            x = i * TILE_SIZE;
            y = 0;
        }
        else if (i < 20) {  // Right column
            x = (numTilesPerSide - 1) * TILE_SIZE;
            y = (i - 10) * TILE_SIZE;
        }
        else if (i < 30) {  // Bottom row
            x = (numTilesPerSide - 1 - (i - 20)) * TILE_SIZE;
            y = (numTilesPerSide - 1) * TILE_SIZE;
        }
        else {  // Left column
            x = 0;
            y = (numTilesPerSide - 1 - (i - 30)) * TILE_SIZE;
        }

        // Save coordinates into the tile
        board[i].x = x;
        board[i].y = y;

        // Draw the outer border of the tile (black)
        SDL_Rect borderRect = { x, y, TILE_SIZE, TILE_SIZE };
        SDL_SetRenderDrawColor(gameInstance->renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(gameInstance->renderer, &borderRect);

        // Draw the inner part of the tile
        SDL_Rect innerRect = { x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4 };

        // Set tile color based on its type
        switch (board[i].type) {
        case TileType::START:
            SDL_SetRenderDrawColor(gameInstance->renderer, 0, 255, 0, 255);  // Green
            break;
        case TileType::JAIL:
        case TileType::GO_TO_JAIL:
            SDL_SetRenderDrawColor(gameInstance->renderer, 128, 128, 128, 255);  // Gray
            break;
        case TileType::FREE_PARKING:
            SDL_SetRenderDrawColor(gameInstance->renderer, 255, 165, 0, 255);  // Orange
            break;
        case TileType::CHANCE:
            SDL_SetRenderDrawColor(gameInstance->renderer, 255, 0, 0, 255);  // Red
            break;
        case TileType::COMMUNITY_CHEST:
            SDL_SetRenderDrawColor(gameInstance->renderer, 255, 255, 0, 255);  // Yellow
            break;
        case TileType::BEACH:
            SDL_SetRenderDrawColor(gameInstance->renderer, 0, 127, 255, 255);  // Blue
            break;
        case TileType::INCOME_TAX:
            SDL_SetRenderDrawColor(gameInstance->renderer, 0, 127, 255, 0);  
            break;
        case TileType::LUXURY_TAX:
            SDL_SetRenderDrawColor(gameInstance->renderer, 0, 127, 127, 0); 
            break;
        default:
            SDL_SetRenderDrawColor(gameInstance->renderer, 255, 255, 255, 255);  // White
        }

        SDL_RenderFillRect(gameInstance->renderer, &innerRect);

        // Render the tile name
        SDL_Color textColor = { 0, 0, 0 };  // Black
        gameInstance->renderText(board[i].name, x + 5, y + 5, textColor);
    }
}

void Board::renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y) {
    SDL_Rect renderQuad = { x, y, 50, 50 };
    SDL_RenderCopy(renderer, houseTexture, nullptr, &renderQuad);
}

void Board::createBoard() {
    board.resize(NUM_TILES);
    setupTilePositions();
    setupSpecialTiles();
}
