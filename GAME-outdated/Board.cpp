#include <iostream>
#include <string>
#include <algorithm>  
#include <random> 
#include "Player.h"
#include "Board.h"
#include "Constants.h"
#include "TileBuilder.h"
#include "Game.h"

Board::Board(Game* gameInstance) : gameInstance(gameInstance), board(NUM_TILES) {}

void Board::createBoard() {
    board.resize(NUM_TILES); // Khởi tạo kích thước cho board
    setupSpecialTiles();
}

void Board::handleChanceEvent(const std::shared_ptr<Player>& player) {
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<> dis(0, gameInstance->getChanceEvents().size() - 1);
    int eventIndex = dis(gen);

    if (eventIndex >= 0 && eventIndex < gameInstance->getChanceEvents().size()) {
        gameInstance->getChanceEvents()[eventIndex](*player); // Dereference shared_ptr
    }
    else {
        std::cerr << "Error: Invalid chance event index." << std::endl;
    }
}

void Board::handleWorldsEvent(const std::shared_ptr<Player>& player) {
    std::cout << player->getName() << " landed on Worlds!" << std::endl;

    // Lấy danh sách các ô đất mà người chơi sở hữu
    const auto ownedProperties = player->getOwnedProperties(); // Sử dụng auto

    // Nếu người chơi không sở hữu ô đất nào
    if (ownedProperties.empty()) {
        std::cout << "You don't own any properties yet." << std::endl;
        return;
    }

    // Hiển thị danh sách các ô đất
    std::cout << "Choose a property to increase its value:" << std::endl;
    for (size_t i = 0; i < ownedProperties.size(); ++i) {
        std::cout << i + 1 << ". " << ownedProperties[i]->getName() << std::endl;
    }

    // Nhận lựa chọn của người chơi
    int choice;
    std::cin >> choice;

    // Xử lý lựa chọn
    if (choice > 0 && choice <= ownedProperties.size()) {
        Tile* selectedTile = ownedProperties[choice - 1];
        selectedTile->setValueMultiplier(selectedTile->getValueMultiplier() * 2); // Tăng hệ số nhân lên gấp đôi
        player->setWorldsUsed(player->getWorldsUsed() + 1); // Tăng số lần sử dụng "Worlds"

        std::cout << "The value of " << selectedTile->getName() << " has been doubled!" << std::endl;
        std::cout << "Multiplier: " << selectedTile->getValueMultiplier() << std::endl;
    }
    else {
        std::cout << "Invalid choice." << std::endl;
    }
}

void Board::applyTax(Player& player, Tile& tile) {
    if (tile.getTileType() == TileType::TAX) {
        int totalHouseValue = 0;

        // Tính tổng giá trị nhà của người chơi
        for (const Tile* p : player.getProperties()) {
            totalHouseValue += p->getNumHouses() * p->getHousePrice();
        }

        // Tính tiền thuế (10% tổng giá trị nhà)
        int taxAmount = 0.1 * totalHouseValue;
        player.setMoney(player.getMoney() - taxAmount);

        std::cout << player.getName() << " paid $" << taxAmount << " in taxes at " << tile.getName() << std::endl;
    }
}

void Board::setupSpecialTiles() {
        // Các ô sự kiện
        board[0] = TileBuilder()
            .withName("Start")
            .withType(TileType::START)
            .withOnLand([this](const std::shared_ptr<Player>& player) { // Thay đổi kiểu dữ liệu
            player->setMoney(player->getMoney() + 200);
                })
            .build();

        board[8] = TileBuilder()
            .withName("Lost Island")
            .withType(TileType::LOST_ISLAND)
            .withOnLand([this](const std::shared_ptr<Player>& player) { // Thay đổi kiểu dữ liệu
            player->setIsOnLostIsland(true);
            std::cout << player->getName() << " is stranded on Lost Island!" << std::endl;
                })
            .build();

        board[16] = TileBuilder()
            .withName("Worlds")
            .withType(TileType::WORLDS)
            .withOnLand([this](const std::shared_ptr<Player>& player) { // Thay đổi kiểu dữ liệu
            this->handleWorldsEvent(player); // Không cần shared_from_this() nữa
                })
            .build();

        board[24] = TileBuilder()
            .withName("World Tour")
            .withType(TileType::WORLD_TOUR)
            .withOnLand([this](const std::shared_ptr<Player>& player) { // Thay đổi kiểu dữ liệu
            player->setOnWorldTour(true);
            std::cout << player->getName() << " activated World Tour!" << std::endl;
                })
            .build();
    
        // Các ô cơ hội 
        for (int i : {12, 20, 28}) {
            board[i] = TileBuilder()
                .withName("Chance")
                .withType(TileType::CHANCE)
                .withOnLand([this](const std::shared_ptr<Player>& player) { // Thay đổi kiểu dữ liệu
                this->handleChanceEvent(player); // Không cần shared_from_this() nữa
                    })
                .build();
        }

        // Các ô bãi biển
        for (int i : {4, 14, 18, 25}) {
            board[i] = TileBuilder()
                .withName("Beach")
                .withCost(200)
                .withType(TileType::BEACH)
                .withHouseMax(1)
                .build();
        }
        // Các ô đất thường
        struct PropertyInfo {
            std::string name;
            int cost;
            ColorGroup colorGroup;
        };
        std::vector<PropertyInfo> properties = {
            {"GRANADA", 120, ColorGroup::BROWN},
            {"SEVILLE", 120, ColorGroup::BROWN},
            {"MADRID", 150, ColorGroup::BROWN},
            {"MADRID", 150, ColorGroup::BROWN},
            {"MA CAO", 170, ColorGroup::PINK},
            {"BEIJING", 200, ColorGroup::PINK},
            {"SHANGHAI", 220, ColorGroup::PINK},
            {"VENICE", 250, ColorGroup::DARK_BLUE},
            {"MILAN", 270, ColorGroup::DARK_BLUE},
            {"ROME", 300, ColorGroup::DARK_BLUE},
            {"HAMBURG", 330,  ColorGroup::ORANGE},
            {"BERLIN", 360,  ColorGroup::ORANGE},
            {"LONDON", 400,  ColorGroup::YELLOW},
            {"SYDNEY", 420,  ColorGroup::YELLOW},
            {"CHICAGO", 450,ColorGroup::NONE},
            {"LAS VEGAS", 470, ColorGroup::NONE},
            {"NEW YORK", 500, ColorGroup::NONE},
            {"LYON", 520,  ColorGroup::GREEN},
            {"PARIS", 560, ColorGroup::GREEN},
            {"OSAKA", 580, ColorGroup::LIGHT_BLUE},
            {"TOKYO", 650, ColorGroup::LIGHT_BLUE}
        };
    
    // Tạo các ô đất thường bằng vòng lặp
    for (size_t i = 0; i < properties.size(); ++i) {
        PropertyInfo info = properties[i];
        board[i + 1] = TileBuilder() // i + 1 vì ô đầu tiên là "Start"
            .withName(info.name)
            .withCost(info.cost)
            .withType(TileType::PROPERTY)
            .withHouseMax(4) // Giả sử tất cả các ô đất thường đều có thể xây tối đa 4 nhà
            .withColorGroup(info.colorGroup)
            .withOwnerName("")
            .build();
    }
}

void Board::renderPlayerAt(std::shared_ptr<Player> player, int x, int y) {
    SDL_Texture* playerTexture = player->getSprite();
    gameInstance->drawPlayer(playerTexture, player->getX(), player->getY());
}

void Board::renderPlayers() {
    if (gameInstance == nullptr) return;
    for (const auto& player : gameInstance->getPlayers()) {
        Tile& tile = board[player->getPosition()];

        // Skip rendering if no players are on the tile
        if (tile.getPlayersOnTile().empty()) continue;

        // Calculate the base position (center) of the tile
        int baseX = tile.getX() + TILE_SIZE / 2;  // Center X
        int baseY = tile.getY() + TILE_SIZE / 2;  // Center Y
        int numPlayers = tile.getPlayersOnTile().size();

        // Vector to store positions of players for collision checking
        std::vector<std::pair<int, int>> playerPositions(numPlayers);

        // Calculate and store player positions
        for (int i = 0; i < numPlayers; ++i) {
            std::shared_ptr<Player> p = tile.getPlayersOnTile()[i];

            // Offset based on player's index
            float angle = (360.0f / numPlayers) * i;  // Angle for each player
            int radius = 20;  // Radius of offset

            // Calculate x, y coordinates with offset based on angle and radius
            int offsetX = static_cast<int>(radius * cos(angle * M_PI / 180.0f));
            int offsetY = static_cast<int>(radius * sin(angle * M_PI / 180.0f));

            // Store position for collision checking
            playerPositions[i] = { baseX + offsetX, baseY + offsetY };
        }

        // Check for collisions
        for (int i = 0; i < numPlayers; ++i) {
            for (int j = i + 1; j < numPlayers; ++j) {
                // Simple distance check for collision
                int dx = playerPositions[i].first - playerPositions[j].first;
                int dy = playerPositions[i].second - playerPositions[j].second;
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < 30) {  
                    int offsetX = 15; // Example offset
                    playerPositions[j].first += offsetX; 
                }
            }
        }

        // Render players with updated positions
        for (int i = 0; i < numPlayers; ++i) {
            std::shared_ptr<Player> p = tile.getPlayersOnTile()[i];
            renderPlayerAt(p, playerPositions[i].first, playerPositions[i].second);
        }
    }
}

void Board::calculateTilePosition(int i, int& x, int& y) {
    switch (i) {
    case 0:  // Góc trên bên trái (START)
        x = 0;
        y = 0;
        break;
    case 8:  // Góc trên bên phải
        x = TILE_SIZE * numTilesPerSide;
        y = 0;
        break;
    case 16: // Góc dưới bên phải
        x = TILE_SIZE * numTilesPerSide;
        y = TILE_SIZE * numTilesPerSide;
        break;
    case 24: // Góc dưới bên trái
        x = 0;
        y = TILE_SIZE * numTilesPerSide;
        break;
    default:
        if (i > 0 && i < 8) { // Các ô trên đường trên (từ ô 1 đến 6)
            x = TILE_SIZE * i;
            y = 0;
        }
        else if (i > 8 && i < 16) { // Các ô trên đường bên phải (từ ô 8 đến 14)
            x = TILE_SIZE * numTilesPerSide;
            y = TILE_SIZE * (i - 8);
        }
        else if (i > 16 && i < 24) { // Các ô trên đường dưới (từ ô 16 đến 22)
            x = TILE_SIZE * (24 - i);
            y = TILE_SIZE * numTilesPerSide;
        }
        else if (i > 24 && i < 32) { // Các ô trên đường bên trái (từ ô 24 đến 31)
            x = 0;
            y = TILE_SIZE * (32 - i);
        }
        break;
    }
}

void Board::renderBoards() {
    for (int i = 0; i < NUM_TILES; i++) {
        int x, y;
        calculateTilePosition(i, x, y); // Tính toán tọa độ

        // Cập nhật tọa độ của ô đất
        board[i].setPosition(x, y);

        // Vẽ ô viền của từng ô
        SDL_Rect borderRect = { x, y, TILE_SIZE, TILE_SIZE };
        SDL_SetRenderDrawColor(gameInstance->getRenderer(), 0, 0, 0, 255);  // Màu viền là màu đen
        SDL_RenderFillRect(gameInstance->getRenderer(), &borderRect);

        // Vẽ phần bên trong của ô
        SDL_Rect innerRect = { x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4 };

        // Thiết lập màu sắc của ô dựa trên loại ô (TileType)
        if (board[i].getTileType() == TileType::PROPERTY) {
            // Chọn màu dựa trên nhóm màu của ô thuộc tính
            switch (board[i].getColorGroup()) {
            case ColorGroup::BROWN:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 150, 75, 0, 255);
                break;
            case ColorGroup::DARK_BLUE:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 0, 0, 139, 255);
                break;
            case ColorGroup::GREEN:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 0, 128, 0, 255);
                break;
            case ColorGroup::LIGHT_BLUE:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 173, 216, 230, 255);
                break;
            case ColorGroup::ORANGE:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 165, 0, 255);
                break;
            case ColorGroup::YELLOW:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 255, 0, 255);
                break;
            case ColorGroup::PINK:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 192, 203, 255);
                break;
            default:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 255, 255, 255);
                break;
            }
        }
        else {
            // Thiết lập màu mặc định cho các ô không phải ô thuộc tính
            switch (board[i].getTileType()) {
            case TileType::START:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 0, 255, 0, 255);  // Xanh lá
                break;
            case TileType::LOST_ISLAND:
            case TileType::WORLD_TOUR:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 128, 128, 128, 255);  // Màu xám
                break;
            case TileType::WORLDS:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 255, 255, 255);  // Màu cam
                break;
            case TileType::CHANCE:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 0, 0, 255);  // Màu đỏ
                break;
            case TileType::BEACH:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 0, 127, 255, 255);  // Màu xanh dương
                break;
            case TileType::TAX:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 0, 0, 255);  // Màu đỏ
                break;
            default:
                SDL_SetRenderDrawColor(gameInstance->getRenderer(), 255, 255, 255, 255);  // Màu trắng
                break;
            }
        }

        SDL_RenderFillRect(gameInstance->getRenderer(), &innerRect);
        if (board[i].getTileType() == TileType::PROPERTY && board[i].getNumHouses() > 0) {
            // Tính toán tọa độ (x, y) cho ngôi nhà
            int houseX = x + 10; // Ví dụ: cách mép trái ô đất 10 pixel
            int houseY = y + 10; // Ví dụ: cách mép trên ô đất 10 pixel
            for (int j = 0; j < board[i].getNumHouses(); ++j) {
                renderHouse(gameInstance->getRenderer(), gameInstance->getHouseTexture(), houseX, houseY);
                houseX += 15; // Ví dụ: khoảng cách giữa các ngôi nhà là 15 pixel
            }
        }
        // Hiển thị tên của ô
        SDL_Color textColor = { 0, 0, 0 };  // Màu chữ là màu đen
        gameInstance->renderText(board[i].getName(), x + 5, y + 5, textColor);  // Render text lên ô
    }
}

void Board::renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y) {
    SDL_Rect renderQuad = { x, y, 50, 50 };
    SDL_RenderCopy(renderer, houseTexture, nullptr, &renderQuad);
}


