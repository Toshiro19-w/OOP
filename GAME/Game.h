#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Constants.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <functional>

class Game {
public:
    Game(const std::vector<std::string>& playerNames);
    ~Game();

    void run();

private:
    void initializeSDL();
    void createBoard();
    void initPlayers();
    void cleanup();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void renderText(const std::string& message, int x, int y, SDL_Color color);
    void renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y);
    void drawPlayer(SDL_Texture* texture, int x, int y);
    void renderPlayerAt(Player* player, int x, int y);
    void renderPlayers();
    int rollDice();
    void tryToLeaveJail(Player& player);
    void nextTurn();
    bool rolledDoubles();
    bool paidFee(Player& player, int jailFee);
    SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);

    // Các phương thức mua
    void buyHouse(Player& player, Tile& tile);
    void buyBeach(Player& player, Tile& tile);

    // Thành viên
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* houseTexture;
    std::vector<Player> players;
    std::vector<Tile> board;
    std::vector<std::function<void(Player&)>> chanceEvents;
    std::vector<std::function<void(Player&)>> communityChestEvents;
    int currentPlayerIndex;
};

#endif  // GAME_H