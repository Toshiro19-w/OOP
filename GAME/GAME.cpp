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

Game::Game(const std::vector<std::string>& playerNames) : currentPlayerIndex(0) {
    initializeSDL();
    board = new Board(this); // Khởi tạo Board và truyền con trỏ đến Game
    board->createBoard(); // Gọi phương thức tạo bảng
    for (const auto& name : playerNames) {
        players.emplace_back(name, 1500);  // Khởi tạo mỗi người chơi với 1500 tiền
    }
    initPlayers();
    currentPlayerIndex = 0;
    setupChanceEvents();
    setupCommunityChestEvents();
}

Game::~Game() {
    delete board;
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

void Game::setupChanceEvents() {
    chanceEvents = {
        [](Player& player) {
            std::cout << player.name << " draws a Chance card: Move to Start." << std::endl;
            player.position = 0;
            player.money += 200;
        },
        [](Player& player) {
            std::cout << player.name << " draws a Chance card: Pay fine of 50." << std::endl;
            player.money -= 50;
        },
        [](Player& player) {
            std::cout << player.name << " draws a Chance card: Get out of jail free!" << std::endl;
            player.hasGetOutOfJailFreeCard = true;
        }
        // thêm cơ hội nếu cần...
    };
}

void Game::setupCommunityChestEvents() {
    communityChestEvents = {
        [](Player& player) {
            std::cout << player.name << " draws a Community Chest card: Earn 100." << std::endl;
            player.money += 100;
        },
        [](Player& player) {
            std::cout << player.name << " draws a Community Chest card: Pay 50 for school fees." << std::endl;
            player.money -= 50;
        },
        [this](Player& player) {
            std::cout << player.name << " draws a Community Chest card: Go to jail!" << std::endl;
            board->sendPlayerToJail(player);
        }
        // thêm khí vận nếu cần...
    };
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
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();  
    Player& currentPlayer = players[currentPlayerIndex];

    if (currentPlayer.isInJail) {
        if (currentPlayer.turnsInJail > 0) {
            std::cout << currentPlayer.name << " is in jail for " << currentPlayer.turnsInJail << " more turn(s)." << std::endl;
            currentPlayer.turnsInJail--;
        }

        // Nếu hết lượt, thả player ra tù
        if (currentPlayer.turnsInJail == 0) {
            std::cout << currentPlayer.name << " is released from jail." << std::endl;
            currentPlayer.releaseFromJail();
        }

        // Skip lượt của player nếu họ vẫn ở trong tù
        if (currentPlayer.isInJail) {
            std::cout << currentPlayer.name << "'s turn is skipped." << std::endl;
            nextTurn();
        }
    }
    else {
        std::cout << currentPlayer.name << "'s turn." << std::endl;
    }
}

void Game::update(float deltaTime) {
    Player& currentPlayer = players[currentPlayerIndex];

    // Nếu player di chuyển, cập nhật vị trí của họ
    if (currentPlayer.isMoving) {
        currentPlayer.updatePosition(deltaTime, players);

        // Kiểm tra xem player đã đến đích chưa
        if (!currentPlayer.isMoving) {
            std::cout << currentPlayer.name << " has reached the target position." << std::endl;

            // Khi mà player đã đến đích, chuyển lượt người chơi
            nextTurn();
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    board->renderBoards();
    board->renderPlayers();
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
            exit(0);
        }
        else if (e.type == SDL_KEYDOWN) {
            handlePlayerActions(e);
        }
    }
}

void Game::handlePlayerActions(const SDL_Event& e) {
    Player& currentPlayer = players[currentPlayerIndex];

    if (e.key.keysym.sym == SDLK_SPACE) {
        // Allow rolling dice only when the player has reached their destination
        if (!currentPlayer.isMoving && currentPlayer.canRollDice) {
            int diceRoll = rollDice();  // Roll the dice
            std::cout << "\n" << currentPlayer.name << " rolled a " << diceRoll << std::endl;

            currentPlayer.move(diceRoll, board->board);  // Move player
            currentPlayer.isMoving = true;        // Player starts moving
            currentPlayer.canRollDice = false;    // Disable further dice rolls for this turn
        }
        else {
            std::cout << currentPlayer.name << " is still moving or cannot roll dice yet." << std::endl;
        }
    }
    else if (e.key.keysym.sym == SDLK_h) {
        Tile& currentTile = board->board[currentPlayer.position];

        if (currentPlayer.canBuyHouse(currentTile)) {
            board->buyHouse(currentPlayer, currentTile);  // Buy a house
        }
        else {
            std::cout << "You cannot buy a house on this tile." << std::endl;
        }
    }
    else if (e.key.keysym.sym == SDLK_b) {
        Tile& currentTile = board->board[currentPlayer.position];
        board->buyBeach(currentPlayer, currentTile);
    }
    else if (e.key.keysym.sym == SDLK_i) currentPlayer.displayInfo();
    else if (e.key.keysym.sym == SDLK_s) {
        Tile& currentTile = board->board[currentPlayer.position];
        board->sellHouse(currentPlayer, currentTile);
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

void Game::handlePlayerInJail(Player& player) {
    if (player.isInJail) {
        if (player.jailTurns <= 0) {
            player.isInJail = false;
            std::cout << player.name << " is released from jail!" << std::endl;
        }
        else {
            std::cout << player.name << " remains in jail for " << player.jailTurns << " more turns." << std::endl;
        }
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

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (true) {
        handleEvents();

        Player& currentPlayer = players[currentPlayerIndex];
        if (currentPlayer.isInJail) {
            handlePlayerInJail(currentPlayer);
            continue; // Bỏ qua cập nhật người chơi này
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
        // Thêm texture mặc định nếu có
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 100); // Texture mặc định kích thước 100x100
    }
    else {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return texture;
}

