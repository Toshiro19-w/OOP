#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Constants.h"
#include "Board.h"
#include "Tile.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <functional>

class Board;

class Game {
public:
    Game(const std::vector<std::string>& playerNames); //Consturstor
    ~Game();

    void run();
    void initializeSDL();
    void initPlayers();
    void cleanup();

    void handleEvents();
    void handlePlayerActions(const SDL_Event& e);
    void update(float deltaTime);

    void render();
    void renderText(const std::string& message, int x, int y, SDL_Color color);
    void drawPlayer(SDL_Texture* texture, int x, int y);

    int rollDice();
    void handlePlayerInJail(Player& player);
    void tryToLeaveJail(Player& player);
    void nextTurn(); // chuyển lượt người chơi

    bool rolledDoubles(); // Nổ đôi giống nhau
    bool paidFee(Player& player, int jailFee); // Trả phí

    SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);

    TTF_Font* font;
    SDL_Texture* houseTexture;
    std::vector<Player> players;
    int currentPlayerIndex;
    Board* board;

    void setupChanceEvents();
    void setupCommunityChestEvents();
    std::vector<std::function<void(Player&)>> chanceEvents;
    std::vector<std::function<void(Player&)>> communityChestEvents;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif  // GAME_H