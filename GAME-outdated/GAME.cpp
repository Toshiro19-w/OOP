#include "Game.h"
#include "Player.h"
#include "Constants.h"
#include "Board.h"
#include "Tile.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());

Game::Game(const std::vector<std::string>& playerNames) : currentPlayerIndex(0) {
    initializeSDL();
    for (const auto& name : playerNames) {
        players.emplace_back(std::make_shared<Player>(name, STARTING_MONEY, this));
    }
    initPlayers();
    currentPlayerIndex = 0;
    setupChanceEvents();

    board = std::make_unique<Board>(this);
    board->createBoard(); // Gọi hàm createBoard để thiết lập bàn cờ
}

Game::~Game() {
    cleanup();
}

void Game::quitGame() {
    isRunning = false;
    SDL_Quit();
}

void Game::initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        exit(1);
    }
    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
        exit(1);
    }
    // Tạo cửa sổ và renderer
    window = SDL_CreateWindow("Monopoly", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
    // Tải phông chữ
    font = TTF_OpenFont("WorkSans-Italic.ttf", 15); // Đường dẫn tới phông chữ
    if (font == nullptr) {
        cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
    }
    // Tải các textures khác
    houseTexture = loadTexture(HOUSE_TEXTURE_PATH, renderer);
    if (houseTexture == nullptr) {
        cerr << "Failed to load house texture!" << endl;
    }
}

void Game::initPlayers() {
    std::vector<std::string> playerTextures = { "Player1.png", "Player2.png", "Player3.png", "Player4.png" };
    for (size_t i = 0; i < players.size(); ++i) {
        SDL_Texture* texture = loadTexture(playerTextures[i], renderer);
        players[i]->setSprite(texture);
    }
}

void Game::setupChanceEvents() {
    chanceEvents = {
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Move to Start." << std::endl;
            player.setPosition(0);
            player.setMoney(player.getMoney() + 200);
            player.updateTargetPosition();
            player.setTargetPosition(player.getTargetX(), player.getTargetY());
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Pay fine of 50." << std::endl;
            player.setMoney(player.getMoney() - 50);
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Go to Lost IsLand." << std::endl;
            player.setPosition(8);
            player.setIsOnLostIsland(true);
            player.updateTargetPosition();
            player.setTargetPosition(player.getTargetX(), player.getTargetY());
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Advance to the nearest Beach" << std::endl;
            // Thêm logic để di chuyển người chơi đến ô đường sắt gần nhất
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Advance to the World Tour" << std::endl;
            player.setPosition(24);
            player.updateTargetPosition();
            player.setTargetPosition(player.getTargetX(), player.getTargetY());
            std::cout << player.getName() << " moved to World Tour" << std::endl;
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Bank pays you dividend of $50." << std::endl;
            player.setMoney(player.getMoney() + 50);
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: A map which is can help you a lots..." << std::endl;

        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Go back 3 spaces." << std::endl;

            // Tính toán vị trí mới sau khi lùi 3 bước
            int newPosition = (player.getPosition() - 3 + NUM_TILES) % NUM_TILES;

            // Di chuyển người chơi đến vị trí mới
            player.setPosition(newPosition);

            // Cập nhật tọa độ hiển thị và trạng thái di chuyển
            player.updateTargetPosition(); // Cập nhật targetX, getTargetY() dựa trên position mới
            player.setTargetPosition(player.getTargetX(), player.getTargetY()); // Bắt đầu di chuyển đến vị trí mới

            std::cout << player.getName() << " moved back to " << player.getPosition() << std::endl;
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Make general repairs on all your property." << std::endl;
            for (auto& otherPlayer : player.getGameInstance()->players) {
            // Kiểm tra xem người chơi có phải là người chơi hiện tại hay không
                if (otherPlayer != player.shared_from_this()) { 
                // Trừ tiền của người chơi khác (ví dụ: $50)
                otherPlayer->setMoney(otherPlayer->getMoney() - 50);
                // Cộng tiền cho người chơi hiện tại
                player.setMoney(player.getMoney() + 50);
            }
        }
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Pay poor tax of $75." << std::endl;
            player.setMoney(player.getMoney() - 75);
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: ADVANCED TO WORLDS" << std::endl;
            player.setPosition(16);
            player.updateTargetPosition();
            player.setTargetPosition(player.getTargetX(), player.getTargetY());
            std::cout << player.getName() << " moved to Worlds" << std::endl;
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: This is Your Birthday!" << std::endl;
            // Thêm logic để thu tiền từ mỗi người chơi (ví dụ: $50)
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: Your building and loan matures." << std::endl;
            player.setMoney(player.getMoney() + 150);
        },
        [](Player player) {
            std::cout << player.getName() << " draws a Chance card: You have won a crossword competition." << std::endl;
            player.setMoney(player.getMoney() + 100);
        },
    };
}

void Game::drawPlayer(SDL_Texture* texture, int x, int y) {
    SDL_Rect playerRect = { x, y, spriteWidth, spriteHeight};
    if (SDL_RenderCopy(renderer, texture, nullptr, &playerRect) != 0) {
        std::cerr << "SDL_RenderCopy error: " << SDL_GetError() << std::endl;
    }
}

int Game::rollDice() {
    std::uniform_int_distribution<> dis(1, 6);
    int dice = dis(gen);

    // Cập nhật các biến lưu trữ kết quả
    auto currentPlayer = players[currentPlayerIndex];
    currentPlayer->setPreviousRoll(currentPlayer->getCurrentRoll());
    currentPlayer->setCurrentRoll(dice);

    return dice;
}

void Game::handleRollDice(std::shared_ptr<Player> currentPlayer) {
    if (!currentPlayer->getIsMoving() && currentPlayer->getCanRollDice()) {
        if (currentPlayer->getOnWorldTour()) { 
            std::cout << currentPlayer->getName() << " is using World Tour to choose a destination." << std::endl;
            currentPlayer->move(0, board->board); 
        }
        else {
            int diceRoll = rollDice();
            std::cout << "\n" << currentPlayer->getName() << " rolled a " << diceRoll << std::endl;
            currentPlayer->move(diceRoll, board->board);
        }
        currentPlayer->setIsMoving(true); 
        currentPlayer->setCanRollDice(false);
    }
    else {
        std::cout << currentPlayer->getName() << " is still moving or cannot roll dice yet." << std::endl;
    }
}

void Game::nextTurn() {
    do {
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    } while (players[currentPlayerIndex]->getState() == PlayerState::Bankrupt); // Bỏ qua người chơi đã phá sản

    std::shared_ptr<Player> currentPlayer = players[currentPlayerIndex];
    std::cout << currentPlayer->getName() << "'s turn." << std::endl;
}

void Game::update(float deltaTime) {
    std::shared_ptr<Player> currentPlayer = players[currentPlayerIndex];

    // Nếu player di chuyển, cập nhật vị trí của họ
    if (currentPlayer->getIsMoving()) {
        currentPlayer->updatePosition(deltaTime, players);

        // Kiểm tra xem player đã đến đích chưa
        if (!currentPlayer->getIsMoving()) {
            std::cout << currentPlayer->getName() << " has reached the target position." << std::endl;

            // Kiểm tra xem người chơi có đang ở trên Lost Island hay không
            if (currentPlayer->getIsOnLostIsland()) {
                currentPlayer->setTurnsOnLostIsland(currentPlayer->getTurnsOnLostIsland() + 1);
                std::cout << currentPlayer->getName() << " is stuck on Lost Island for "
                    << currentPlayer->getTurnsOnLostIsland() << " turns." << std::endl;

                // Kiểm tra điều kiện để thoát khỏi đảo
                if (currentPlayer->getTurnsOnLostIsland() >= 3 || hasRolledDoubles()) {
                    currentPlayer->setIsOnLostIsland(false);
                    currentPlayer->setTurnsOnLostIsland(0);
                    std::cout << currentPlayer->getName() << " escapes Lost Island!" << std::endl;
                }
                else {
                    // Nếu chưa thoát khỏi đảo, kết thúc lượt
                    return;
                }
            }

            // Kiểm tra xem người chơi có bị phá sản sau khi di chuyển hay không
            if (currentPlayer->getMoney() < 0) {
                currentPlayer->setState(PlayerState::Bankrupt);
                std::cout << currentPlayer->getName() << " is bankrupt!" << std::endl;
            }

            // Chuyển lượt người chơi
            nextTurn();
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);    
    SDL_RenderClear(renderer);
    board->renderBoards();
    board->renderPlayers();
    std::shared_ptr<Player> currentPlayer = players[currentPlayerIndex];
    std::string moneyText = "Money: $" + std::to_string(currentPlayer->getMoney());
    renderText(moneyText, 10, 10, { 255, 255, 255 }); // Ví dụ hiển thị ở góc trên bên trái

    SDL_RenderPresent(renderer);
}

void Game::renderText(const std::string& message, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int width = surface->w;
    int height = surface->h;

    SDL_Rect renderQuad = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::cleanup() {
    // Destroy player textures
    for (const auto& player : players) {
        if (player->getSprite()) {
            SDL_DestroyTexture(player->getSprite());
            // Không cần thiết đặt player->sprite = nullptr vì player sẽ bị hủy 
        }
    }

    // Destroy house texture
    if (houseTexture) {
        SDL_DestroyTexture(houseTexture);
        houseTexture = nullptr;
    }

    // Destroy renderer
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // Destroy window
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // Close font and quit TTF
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();

    // Quit SDL
    SDL_Quit();
}

void Game::buyHouse(Tile& tile) {
    std::shared_ptr<Player> currentPlayer = board->gameInstance->players[currentPlayerIndex];

    // Kiểm tra loại ô đất
    if (tile.getTileType() != TileType::PROPERTY) {
        std::cout << "You can only buy a house on regular plots." << std::endl;
        return;
    }

    // Nếu ô đất đã có người khác sở hữu
    if (!tile.getOwnerName().empty() && tile.getOwnerName() != currentPlayer->getName()) {
        std::cout << "This property is already owned by another player." << std::endl;
        return;
    }

    // Nếu người chơi chưa sở hữu ô đất, thêm nó vào danh sách tài sản
    if (!tile.getOwnerName().empty()) {
        tile.setOwnerName(currentPlayer->getName());  // Đặt người chơi làm chủ sở hữu ô đất
        currentPlayer->addProperty(&tile);
    }

    // Kiểm tra khả năng mua nhà
    if (currentPlayer->canBuyHouse(tile) && currentPlayer->getMoney() >= tile.getHousePrice()) {
        currentPlayer->setMoney(currentPlayer->getMoney() - tile.getHousePrice());
        tile.setNumHouses(tile.getNumHouses() + 1);
        std::cout << "You have successfully purchased a house on " << tile.getName() << "." << std::endl;
    }
    else {
        if (tile.getNumHouses() >= MAX_HOUSE) {
            std::cout << "You have reached the limit on the number of houses on this plot." << std::endl;
        }
        else {
            std::cout << (currentPlayer->getMoney() < tile.getHousePrice() ? "You can't afford to buy a house on this plot." : "You cannot buy a house on this plot.") << std::endl;
        }
    }
}

void Game::sellHouse(std::shared_ptr<Player> player) {
    std::vector<Tile*> ownedTiles = getOwnedTilesWithHouses(player);
    if (ownedTiles.empty()) {
        std::cout << player->getName() << " does not own any houses to sell." << std::endl;
        return;
    }

    displayOwnedTiles(player, ownedTiles);
    int choice = getPlayerChoice(ownedTiles.size());

    if (choice == -1) {
        std::cout << "Invalid choice. Please select a valid property number." << std::endl;
        return;
    }

    Tile* selectedTile = ownedTiles[choice - 1];
    sellHouseOnTile(player, selectedTile);
}

std::vector<Tile*> Game::getOwnedTilesWithHouses(std::shared_ptr<Player> player) {
    std::vector<Tile*> ownedTiles;
    for (Tile& tile : board->board) { // Sửa tiles thành board
        if (tile.getOwnerName() == player->getName() && tile.getNumHouses() > 0) { // Sử dụng getter
            ownedTiles.push_back(&tile);
        }
    }
    return ownedTiles;
}

void Game::displayOwnedTiles(std::shared_ptr<Player> player, const std::vector<Tile*>& ownedTiles) {
    std::cout << player->getName() << ", select a property to sell a house from:" << std::endl;
    for (size_t i = 0; i < ownedTiles.size(); ++i) {
        std::cout << i + 1 << ": " << ownedTiles[i]->getName()
            << " (Houses: " << ownedTiles[i]->getNumHouses() << ")" << std::endl;
    }
}

int Game::getPlayerChoice(size_t numChoices) {
    int choice;
    std::cin >> choice;
    if (std::cin.fail() || choice <= 0 || choice > static_cast<int>(numChoices)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1; // Trả về -1 nếu lựa chọn không hợp lệ
    }
    return choice;
}

void Game::sellHouseOnTile(std::shared_ptr<Player> player, Tile* tile) {
    if (tile->getNumHouses() > 0) {
        int sellPrice = tile->getHousePrice() / 2;
        tile->setHousePrice(sellPrice);
        player->setMoney(player->getMoney() + sellPrice); // Sửa lỗi: sử dụng setter
        tile->setNumHouses(tile->getNumHouses() - 1);
        std::cout << player->getName() << " sold a house on " << tile->getName()
            << " for $" << sellPrice << std::endl;
    }
    else {
        std::cout << "No houses to sell on " << tile->getName() << std::endl;
    }
}

void Game::buyBeach(std::shared_ptr<Player> player, Tile& tile) {
    // Kiểm tra nếu người chơi hiện tại đang đứng trên bãi biển
    if (tile.getTileType() == TileType::BEACH) {
        // Kiểm tra nếu bãi biển đã được sở hữu
        if (!tile.getOwnerName().empty()) { // Kiểm tra ownerName
            std::cout << "This beach is already owned by another player." << std::endl;
            return;
        }

        // Kiểm tra người chơi có đủ tiền để mua bãi biển
        if (player->getMoney() >= BEACH_COST) {
            player->setMoney(player->getMoney() - BEACH_COST);
            tile.setOwnerName(player->getName()); 
            player->addProperty(&tile);
            std::cout << player->getName() << " bought the beach: " << tile.getName() << std::endl;

            // Kiểm tra nếu người chơi sở hữu cả 4 bãi biển
            int beachCount = std::count_if(
                player->getProperties().begin(),
                player->getProperties().end(),
                [](const Tile* property) {
                    return property->getTileType() == TileType::BEACH;
                }
            );
            if (beachCount == 4) {
                std::cout << player->getName() << " owns all 4 beaches and wins the game!" << std::endl;
                exit(0); // Kết thúc trò chơi
            }
        }
        else {
            std::cout << player->getName() << " doesn't have enough money to buy the beach!" << std::endl;
        }
    }
    else {
        std::cout << "This is not a beach." << std::endl; // Thông báo nếu không phải ô đất bãi biển
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quitGame();
        }
        else if (e.type == SDL_KEYDOWN) {
            handleKeyPress(e.key.keysym.sym);
        }
    }
}

void Game::handleKeyPress(SDL_Keycode key) {
    std::shared_ptr<Player> currentPlayer = players[currentPlayerIndex];

    if (key == SDLK_SPACE) {
        handleRollDice(currentPlayer);
    }
    else if (key == SDLK_h) {
        Tile& currentTile = board->board[currentPlayer->getPosition()];
        buyHouse(currentTile);
    }
    else if (key == SDLK_b) {
        Tile& currentTile = board->board[currentPlayer->getPosition()];
        buyBeach(currentPlayer, currentTile);
    }
    else if (key == SDLK_i) {
        currentPlayer->displayInfo();
    }
    else if (key == SDLK_l) {
        sellHouse(currentPlayer);
    }
    else if (key == SDLK_p) {
        currentPlayer->printRollHistory();
    }
}

bool Game::hasRolledDoubles() {
    std::uniform_int_distribution<> dis(1, 6);
    int dice1 = dis(gen);  // Xúc xắc 1 (giá trị từ 1 đến 6)
    int dice2 = dis(gen);  // Xúc xắc 2 (giá trị từ 1 đến 6)

    std::cout << "Player rolled: " << dice1 << " and " << dice2 << std::endl;
    return dice1 == dice2;  // Trả về true nếu lăn được "doubles"
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (isRunning) {
        handleEvents();

        std::shared_ptr<Player> currentPlayer = players[currentPlayerIndex];

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        update(deltaTime);
        render();
    }
}

SDL_Texture* Game::loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* texture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    if (!loadedSurface) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        // Load a default texture if the image fails
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 100);
    }
    else {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return texture;
}