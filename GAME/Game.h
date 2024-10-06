#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Player.h"
#include "Tile.h"

// Lớp Game chính
class Game {
public:
    Game();
    ~Game();
    void run();

//private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* houseTexture; 
    std::vector<Player> players;
    int currentPlayerIndex;
    std::vector<Tile> board;
    std::vector<std::function<void(Player&)>> chanceEvents;
    std::vector<std::function<void(Player&)>> communityChestEvents;

    void initializeSDL();
    void createBoard();
    void handleEvents(); // xử lý sự kiện
    void update(float deltaTime);
    void render();
    void cleanup();
    void nextTurn();
    int rollDice();
    void buyHouse(Player& player, Tile& tile);
    void renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y);
    SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);
};

#endif
