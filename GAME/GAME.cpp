#include "Game.h"
#include "Player.h"
#include "Constants.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>

using namespace std;

Game::Game(const std::vector<std::string>& playerNames) : currentPlayerIndex(0) {
    initializeSDL();
    createBoard();
    for (const auto& name : playerNames) {
        players.emplace_back(name, 1500);  // Khởi tạo mỗi người chơi với 1500 tiền
    }
    initPlayers();
    currentPlayerIndex = 0;

    // Khởi tạo các sự kiện cơ hội
    chanceEvents = {
        [](Player& player) {
            cout << "Chance Event: You gained $100!" << endl;
            player.money += 100;
        },
        [](Player& player) {
            cout << "Chance Event: You lost $50!" << endl;
            player.money -= 50;
        },
        [this](Player& player) {
            cout << "Chance Event: Move forward 3 spaces!" << endl;
            player.move(3, board); // Di chuyển tới trước 3 ô
            player.updateTargetPosition();
        },
        [this](Player& player) {
            cout << "Chance Event: Go back 2 spaces!" << endl;
            player.move(-2, board); // Di chuyển lùi 2 ô
            player.updateTargetPosition();
        }
    };

    // Khởi tạo các sự kiện khí vận
    communityChestEvents = {
        [](Player& player) {
            cout << "Community Chest: You gained $200!" << endl;
            player.money += 200;
        },
        [](Player& player) {
            cout << "Community Chest: You lost $100!" << endl;
            player.money -= 100;
        }
    };
}

Game::~Game() {
    cleanup();
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
    font = TTF_OpenFont("WorkSans-Italic-VariableFont_wght-Italic.otf", 15); // Đường dẫn tới phông chữ
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

        players[i].setSprite(texture);  // Gán texture cho người chơi
        //players[i].position = 0;  // Vị trí của ô Start là 0
        //Tile& startTile = board[players[i].position];
        //players[i].setTargetPosition(startTile.x, startTile.y);
    }
}


void Game::drawPlayer(SDL_Texture* texture, int x, int y) {
    SDL_Rect playerRect = { x, y, spriteWidth, spriteHeight };
    if (SDL_RenderCopy(renderer, texture, nullptr, &playerRect) != 0) {
        std::cerr << "SDL_RenderCopy error: " << SDL_GetError() << std::endl;
    }
}

void Game::renderPlayerAt(Player* player, int x, int y) {
    SDL_Texture* playerTexture = player->getSprite();
    drawPlayer(playerTexture, player->getX(), player->getY());
}

void Game::renderPlayers() {
    for (Player& player : players) {
        Tile& tile = board[player.position];
        int numPlayers = tile.playersOnTile.size();

        if (numPlayers == 0) continue;

        for (int i = 0; i < numPlayers; ++i) {
            Player* p = tile.playersOnTile[i];
            int offsetX = (numPlayers > 1) ? i * 10 : 0;
            int offsetY = (numPlayers > 1) ? i * 10 : 0;

            renderPlayerAt(p, tile.x + offsetX, tile.y + offsetY);
        }
    }
}

void Game::createBoard() {
    board.resize(NUM_TILES);

    // Tạo các tọa độ cho 40 ô
    int x = 0, y = 0;
    for (int i = 0; i < NUM_TILES; ++i) {
        // Góc dưới cùng bên trái (0, 0)
        if (i >= 0 && i < 10) {            // Đường đi từ trái sang phải (dưới cùng)
            x = i;
            y = 0;
        }
        else if (i >= 10 && i < 20) {     // Đường đi từ dưới lên trên (phải)
            x = 9;
            y = i - 10;
        }
        else if (i >= 20 && i < 30) {     // Đường đi từ phải sang trái (trên cùng)
            x = 29 - i;
            y = 9;
        }
        else if (i >= 30 && i < 40) {     // Đường đi từ trên xuống dưới (trái)
            x = 0;
            y = 39 - i;
        }
        // Cấu hình các ô đặc biệt
        if (i == 0) {
            board[i] = Tile("Start", 0, 0, START, 0, false, [](Player& player) {
                player.money += 200;
                }, x, y);
        }
        else if (i == 10) {
            board[i] = Tile("Jail", 0, 0, JAIL, 0, false, [](Player& player) {
                if (!player.isInJail) {
                    cout << player.name << " just visiting jail." << endl;
                    player.isMoving = true;
                }
                }, x, y);
        }
        else if (i == 20) {
            board[i] = Tile("Free Parking", 0, 0, FREE_PARKING, 0, false, nullptr, x, y);
        }
        else if (i == 30) {
            board[i] = Tile("Go to Jail", 0, 0, GO_TO_JAIL, 0, false, [](Player& player) {
                if (!player.isInJail) {
                    std::cout << "--------------------------------" << std::endl;
                    std::cout << player.name << " is sent to Jail!" << std::endl;
                }
                }, x, y);
        }
        // Các ô cơ hội và cộng đồng
        else if (i == 7 || i == 22 || i == 36) {
            board[i] = Tile("Chance", 0, 0, CHANCE, 0, false, [this](Player& player) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, chanceEvents.size() - 1);
                int eventIndex = dis(gen);
                chanceEvents[eventIndex](player);
                }, x, y);
        }
        else if (i == 2 || i == 17 || i == 33) {
            board[i] = Tile("ComChest", 0, 0, COMMUNITY_CHEST, 0, false, [this](Player& player) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, communityChestEvents.size() - 1);
                int eventIndex = dis(gen);
                communityChestEvents[eventIndex](player);
                }, x, y);
        }
        // Các ô đất
        else if (i == 5 || i == 15 || i == 25 || i == 35) {
            board[i] = Tile("Beach", 200, 0, BEACH, 1, false, nullptr, x, y);
        }
        else {
            int propertyCost = 100 + (i % 10) * 20;
            board[i] = Tile("Tile " + to_string(i), propertyCost, 0, NORMAL, 4, false, nullptr, x, y);
        }
    }
}

void Game::renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y) {
    SDL_Rect renderQuad = { x, y, 50, 50 }; 
    SDL_RenderCopy(renderer, houseTexture, nullptr, &renderQuad);
}

int Game::rollDice() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 6);
    int dice = dis(gen);

    // Cập nhật các biến lưu trữ kết quả
    players[currentPlayerIndex].previousRoll = players[currentPlayerIndex].currentRoll;
    players[currentPlayerIndex].currentRoll = dice;

    return dice;
}

void Game::nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();  // Move to the next player
    Player& currentPlayer = players[currentPlayerIndex];

    if (currentPlayer.isInJail) {
        if (currentPlayer.turnsInJail > 0) {
            std::cout << currentPlayer.name << " is in jail for " << currentPlayer.turnsInJail << " more turn(s)." << std::endl;
            currentPlayer.turnsInJail--;
        }

        // If the player has completed their jail sentence, release them
        if (currentPlayer.turnsInJail == 0) {
            std::cout << currentPlayer.name << " is released from jail." << std::endl;
            currentPlayer.releaseFromJail();
        }

        // Skip the turn if the player is still in jail
        if (currentPlayer.isInJail) {
            std::cout << currentPlayer.name << "'s turn is skipped." << std::endl;
            nextTurn();  // Skip to the next player
        }
    }
    else {
        // If the player is not in jail, proceed with the game normally
        std::cout << currentPlayer.name << "'s turn." << std::endl;
        // Your existing logic to handle the player's turn...
    }
}


void Game::update(float deltaTime) {
    Player& currentPlayer = players[currentPlayerIndex];

    // If the current player is moving, update their position
    if (currentPlayer.isMoving) {
        currentPlayer.updatePosition(deltaTime, players);

        // Check if they have reached the target
        if (!currentPlayer.isMoving) {
            std::cout << currentPlayer.name << " has reached the target position." << std::endl;

            // Once the player has reached the target, move to the next turn
            nextTurn();
        }
    }
}
// Mua nhà
void Game::buyHouse(Player& player, Tile& tile) {
    // Kiểm tra nếu người chơi hiện tại là người đang thực hiện hành động
    if (&player != &players[currentPlayerIndex]) {
        std::cout << "It's not your turn to buy a house." << std::endl;
        return;
    }

    // Nếu người chơi chưa sở hữu ô đất, thêm nó vào danh sách tài sản
    auto it = std::find(player.properties.begin(), player.properties.end(), &tile);
    if (it == player.properties.end()) {
        player.addProperty(&tile);
    }

    // Kiểm tra loại ô đất và số nhà có thể xây
    if (tile.type != NORMAL) {
        cout << "You can only buy a house on regular plots." << endl;
        return;
    }

    if (player.canBuyHouse(tile) && player.money >= tile.housePrice) {
        player.money -= tile.housePrice;
        tile.numHouses++;
        cout << "You have successfully purchased a house on " << tile.name << "." << endl;
        player.displayInfo();
    }
    else {
        if (tile.numHouses >= tile.maxHouses) {
            cout << "You have reached the limit on the number of houses on this plot." << endl;
        }
        else if (player.money < tile.housePrice) {
            cout << "You can't afford to buy a house on this plot." << endl;
        }
        else {
            cout << "You cannot buy a house on this plot." << endl;
        }
    }
}
// Mua bãi biển
void Game::buyBeach(Player& player, Tile& tile) {
    // Kiểm tra nếu người chơi hiện tại đang đứng trên bãi biển và nó chưa được sở hữu
    if (tile.type == BEACH && !tile.isOwned) {
        if (player.money >= tile.housePrice) {
            player.money -= tile.housePrice;
            tile.isOwned = true;
            player.addProperty(&tile); // Thêm bãi biển vào danh sách tài sản của người chơi
            std::cout << player.name << " bought the beach: " << std::endl;

            // Kiểm tra nếu người chơi sở hữu cả 4 bãi biển
            int beachCount = 0;
            for (const Tile* property : player.properties) {
                if (property->type == BEACH) {
                    beachCount++;
                }
            }
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

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Thiết lập màu nền đen
    SDL_RenderClear(renderer);

    // Vẽ bàn cờ
    for (int i = 0; i < NUM_TILES; ++i) {
        int x = 0, y = 0;

        // Tính toán vị trí x, y cho từng ô
        if (i < 10) {  // Ô nằm ở cạnh trên
            x = i * TILE_SIZE;
            y = 0;
        }
        else if (i < 20) {  // Ô nằm ở cạnh phải
            x = (numTilesPerSide - 1) * TILE_SIZE;
            y = (i - 10) * TILE_SIZE;
        }
        else if (i < 30) {  // Ô nằm ở cạnh dưới
            x = (numTilesPerSide - 1 - (i - 20)) * TILE_SIZE;
            y = (numTilesPerSide - 1) * TILE_SIZE;
        }
        else {  // Ô nằm ở cạnh trái
            x = 0;
            y = (numTilesPerSide - 1 - (i - 30)) * TILE_SIZE;
        }

        // Lưu tọa độ x, y vào ô (tile)
        board[i].x = x;
        board[i].y = y;

        // Vẽ từng ô theo logic hiện tại
        SDL_Rect borderRect = { x, y, TILE_SIZE, TILE_SIZE };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Đen
        SDL_RenderFillRect(renderer, &borderRect);

        SDL_Rect innerRect = { x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4 };

        switch (board[i].type) {
        case START:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Xanh lá
            break;
        case JAIL:
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);  // Xám
            break;
        case FREE_PARKING:
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);  // Cam
            break;
        case CHANCE:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Đỏ
            break;
        case COMMUNITY_CHEST:
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Vàng
            break;
        case GO_TO_JAIL:
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);  // Xám
            break;
        case BEACH:
            SDL_SetRenderDrawColor(renderer, 0, 127, 255, 255);  // Xanh dương
            break;
        default:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Trắng
        }

        SDL_RenderFillRect(renderer, &innerRect);

        // Vẽ tên của tile
        SDL_Color textColor = { 0, 0, 0 };  // Đen
        renderText(board[i].name, x + 5, y + 5, textColor);  // Vẽ tên ô
    }

    // Gọi renderPlayers để vẽ tất cả các người chơi lên bàn cờ
    renderPlayers();

    // Hiển thị mọi thứ lên màn hình
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

void Game::cleanup() { // Giải phóng bộ nhớ khi kết thúc trò chơi.
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(houseTexture);
    for (Player& player : players) {
        SDL_DestroyTexture(player.getSprite());
    }
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            exit(0);  // Thoát game khi người chơi bấm thoát
        }
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            Player& currentPlayer = players[currentPlayerIndex];

            // Chỉ cho phép người chơi đổ xúc xắc khi họ đã đến vị trí đích
            if (!currentPlayer.isMoving && currentPlayer.canRollDice) {
                int diceRoll = rollDice();  // Đổ xúc xắc
                std::cout << "\n" << currentPlayer.name << " rolled a " << diceRoll << std::endl;

                currentPlayer.move(diceRoll, board);  // Di chuyển
                currentPlayer.isMoving = true;        // Bắt đầu di chuyển
                currentPlayer.canRollDice = false;    // Không cho phép đổ xúc xắc tiếp tục trong lượt này
            }
            else {
                std::cout << currentPlayer.name << " is still moving or cannot roll dice yet." << std::endl;
            }
        }
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_h) {
            Player& currentPlayer = players[currentPlayerIndex];
            Tile& currentTile = board[currentPlayer.position];

            if (currentPlayer.canBuyHouse(currentTile)) {
                buyHouse(currentPlayer, currentTile);  // Mua nhà
            }
            else {
                std::cout << "You cannot buy a house on this tile." << std::endl;
            }
        }
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_b) {
            Player& currentPlayer = players[currentPlayerIndex];
            Tile& currentTile = board[currentPlayer.position];
            buyBeach(currentPlayer, currentTile);
        }
        
    }
}

bool Game::rolledDoubles() {
    // Khởi tạo ngẫu nhiên (nếu chưa thực hiện)
    std::srand(std::time(0));

    int dice1 = std::rand() % 6 + 1;  // Xúc xắc 1 (giá trị từ 1 đến 6)
    int dice2 = std::rand() % 6 + 1;  // Xúc xắc 2 (giá trị từ 1 đến 6)

    std::cout << "Player rolled: " << dice1 << " and " << dice2 << std::endl;

    return dice1 == dice2;  // Trả về true nếu lăn được "doubles"
}

bool Game::paidFee(Player& player, int jailFee) {
    if (player.money >= JAIL_FEE) {
        std::cout << player.name << " has paid " << JAIL_FEE << " to get out of jail." << std::endl;
        player.money -= JAIL_FEE;  // Trừ tiền phí ra tù
        return true;
    }
    else {
        std::cout << player.name << " does not have enough money to pay the jail fee." << std::endl;
        return false;
    }
}

void Game::tryToLeaveJail(Player& player) {
    if (player.isInJail) {
        if (rolledDoubles()) {
            std::cout << player.name << " rolled doubles and is released from jail!" << std::endl;
            player.releaseFromJail();
        }
        else if (paidFee(player, 200)) {  // $200 là phí ra tù
            std::cout << player.name << " paid the fee and is released from jail!" << std::endl;
            player.releaseFromJail();
        }
        else {
            std::cout << player.name << " remains in jail." << std::endl;
        }
    }
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (true) {
        handleEvents();

        // Nếu người chơi đang trong tù, bỏ qua lượt của họ và giảm thời gian giam
        if (players[currentPlayerIndex].isInJail) {
            // Nếu đã hết lượt trong tù, thả họ ra
            if (players[currentPlayerIndex].jailTurns <= 0) {    
                players[currentPlayerIndex].isInJail = false;
                cout << "Player " << currentPlayerIndex + 1 << " is released from jail!" << endl;
            }

            // Không chuyển người chơi trong hàm run, chuyển sau khi người chơi hoàn tất lượt
            continue;  // Bỏ qua cập nhật cho người chơi này và chuyển sang người tiếp theo
        }

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

    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    else {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return texture;
}
