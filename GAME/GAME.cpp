#include "Game.h"
#include "Player.h"
#include "Constants.h"
#include <SDL_image.h>
#include <iostream>
#include <random>

using namespace std;

Game::Game() : currentPlayerIndex(0) {
    initializeSDL();
    createBoard();

    // Thêm 2 người chơi vào vector players
    players.push_back(Player());  // Người chơi 1
    players.push_back(Player());  // Người chơi 2

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
        },
        [this](Player& player) {
            cout << "Chance Event: Go back 2 spaces!" << endl;
            player.move(-2, board); // Di chuyển lùi 2 ô
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

void Game::initializeSDL() { // Khởi tạo SDL và tạo cửa sổ cùng renderer.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        exit(1);
    }

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

    houseTexture = loadTexture(HOUSE_TEXTURE_PATH, renderer);
    if (houseTexture == nullptr) {
        cerr << "Failed to load house texture!" << endl;
        
    }
}

void Game::createBoard() {
    board.resize(NUM_TILES);

    // Các ô đặc biệt
    board[0] = { "Start", 0, "start", [](Player& player) { player.money += 200; } };
    board[10] = { "Jail", 0, "jail", [](Player& player) {
    if (!player.isInJail) {
        // Nếu chỉ là người chơi ghé thăm tù (không bị giam)
        cout << "Just visiting jail." << endl;
        player.isMoving = true; // Vẫn có thể di chuyển ở lượt sau
    }
} };
    board[20] = { "Free Parking", 0, "free_parking", nullptr };
    board[30] = { "Go to Jail", 0, "go_to_jail", [](Player& player) {
    if (!player.isInJail) {
        std::cout << "Go to Jail!" << std::endl;
        player.position = 10; // Đưa người chơi đến vị trí ô nhà tù (ô 10)
        player.isInJail = true; // Đánh dấu người chơi đang ở trong tù
        player.turnsInJail = 3; // Đặt lại số lượt ở tù
        player.isMoving = true; // Dừng di chuyển
        player.canRollDice = true; // Cho phép người chơi tung xúc xắc để thử ra khỏi tù
    }
} };

    // Các ô đất thông thường và thêm các ô cơ hội/khí vận khác
    for (int i = 1; i < NUM_TILES; ++i) {
        if (i == 7 || i == 22 || i == 36) {
            // Ô Cơ Hội (Chance)
            board[i] = { "Chance", 0, "chance", [this](Player& player) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, chanceEvents.size() - 1);
                int eventIndex = dis(gen);
                chanceEvents[eventIndex](player);
            } };
        }
        else if (i == 2 || i == 17 || i == 33) {
            // Ô Khí Vận (Community Chest)
            board[i] = { "Community Chest", 0, "community_chest", [this](Player& player) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, communityChestEvents.size() - 1);
                int eventIndex = dis(gen);
                communityChestEvents[eventIndex](player);
            } };
        }
        else if (i != 10 && i != 20 && i != 30) {
            // Ô đất thông thường
            int propertyCost = 100 + (i % 10) * 20;
            board[i] = { "Tile " + to_string(i), propertyCost, "normal", nullptr };
        }
    }
}

void Game::renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y) {
    SDL_Rect renderQuad = { x, y, 50, 50 }; // Adjust size as needed
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

    cout << "Player " << currentPlayerIndex + 1 << " rolled: " << dice << endl;
    return dice;
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            exit(0);
        }
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            // Người chơi hiện tại đổ xúc xắc
            Player& currentPlayer = players[currentPlayerIndex];
            if (currentPlayer.canRollDice) {
                int diceRoll = rollDice();
                currentPlayer.move(diceRoll, board);

                // Chuyển sang người chơi tiếp theo sau khi di chuyển và chờ
                currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
            }
        } 
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_h) {
            Player& currentPlayer = players[currentPlayerIndex];
            Tile& currentTile = board[currentPlayer.position];

            // Mua nhà nếu đứng trên ô đất và có thể mua
        if (currentPlayer.canBuyHouse(currentTile)) {
                buyHouse(currentPlayer, currentTile);
        }
        else {
                cout << "Cannot buy a house on this property." << endl;
             }
        }
    }
}

void Game::nextTurn() {
    // Di chuyển đến người chơi tiếp theo
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

    if (players[currentPlayerIndex].isInJail && players[currentPlayerIndex].turnsInJail > 0) {
        std::cout << "Player " << currentPlayerIndex << " is in jail, skipping turn." << std::endl;
        players[currentPlayerIndex].turnsInJail--;
        nextTurn(); // Gọi lại hàm để bỏ qua người chơi trong tù
    }
    else {
        std::cout << "It's Player " << currentPlayerIndex << "'s turn!" << std::endl;
    }
}

void Game::update(float deltaTime) {
    for (auto& player : players) {
        player.updatePosition(deltaTime);
    }
}

// Mua nhà
void Game::buyHouse(Player& player, Tile& tile) {
    // Kiểm tra nếu người chơi có thể mua nhà trên ô đất này
    if (player.canBuyHouse(tile) && player.money >= tile.housePrice) {
        player.money -= tile.housePrice;
        tile.numHouses++;
        cout << "You have successfully bought a house on " << tile.name << endl;
    } else {
        cout << "You cannot buy a house on this property." << endl;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < board.size(); ++i) {
        int x, y;
        if (i < 10) {
            x = i * TILE_SIZE;
            y = 0;
        }
        else if (i < 20) {
            x = SCREEN_WIDTH - TILE_SIZE;
            y = (i - 10) * TILE_SIZE;
        }
        else if (i < 30) {
            x = SCREEN_WIDTH - (i - 20) * TILE_SIZE - TILE_SIZE;
            y = SCREEN_HEIGHT - TILE_SIZE;
        }
        else {
            x = 0;
            y = SCREEN_HEIGHT - (i - 30) * TILE_SIZE - TILE_SIZE;
        }

        SDL_Rect tileRect = { x, y, TILE_SIZE, TILE_SIZE };

        if (board[i].type == "start") {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Xanh lá
        }
        else if (board[i].type == "jail") {
            SDL_SetRenderDrawColor(renderer, 105, 0, 0, 255); // Đỏ
        }
        else if (board[i].type == "free_parking") {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Vàng
        }
        else if (board[i].type == "chance") {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Cơ hội: Xanh dương
        }
        else if (board[i].type == "community_chest") {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Khí vận: Cam
        }
        else if (board[i].type == "go_to_jail") {
            SDL_SetRenderDrawColor(renderer, 0, 165, 0, 255); // Vào tù
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Trắng
        }

        SDL_RenderFillRect(renderer, &tileRect);

        // Vẽ nhà nếu có
        if (board[i].numHouses > 0) {
            for (int h = 0; h < board[i].numHouses; ++h) {
                renderHouse(renderer, houseTexture, x + h * 15, y + 15); // Điều chỉnh tọa độ để không đè lên nhau
            }
        }
    }

    // Vẽ tất cả người chơi
    for (const Player& player : players) {
        SDL_Rect playerRect = { static_cast<int>(player.x), static_cast<int>(player.y), PLAYER_SIZE, PLAYER_SIZE };

        // Thiết lập màu sắc khác nhau cho từng người chơi
        SDL_SetRenderDrawColor(renderer, 165, 105, 0, 255);
        SDL_RenderFillRect(renderer, &playerRect);
    }
    SDL_RenderPresent(renderer);
}


void Game::cleanup() { // Giải phóng bộ nhớ khi kết thúc trò chơi.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(houseTexture);
    SDL_Quit();
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (true) {
        handleEvents();

        // Nếu người chơi đang trong tù, bỏ qua lượt của họ và giảm thời gian giam
        if (players[currentPlayerIndex].isInJail) {
            players[currentPlayerIndex].jailTurns--;

            // Nếu đã hết lượt trong tù, thả họ ra
            if (players[currentPlayerIndex].jailTurns <= 0) {
                players[currentPlayerIndex].isInJail = false;
                cout << "Player " << currentPlayerIndex + 1 << " is released from jail!" << endl;
            }

            // Chuyển sang người chơi tiếp theo
            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
            continue;  // Bỏ qua cập nhật cho người chơi này và chuyển sang người tiếp theo
        }
        if (board[players[currentPlayerIndex].position].type == "go_to_jail") {
            cout << "Player " << currentPlayerIndex + 1 << " is going to jail!" << endl;
            players[currentPlayerIndex].isInJail = true;
            players[currentPlayerIndex].jailTurns = 3;  // Họ sẽ bị giam trong 3 lượt
            players[currentPlayerIndex].position = 10;  // Đưa người chơi đến ô nhà tù
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        update(deltaTime);
        render();

        // Chuyển sang người chơi tiếp theo
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    }
}

SDL_Texture* Game::loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
