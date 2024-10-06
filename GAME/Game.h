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
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* houseTexture; 
    std::vector<Player> players;
    int currentPlayerIndex;
    std::vector<Tile> board;
    SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);

    // Danh sách sự kiện cơ hội và khí vận
    std::vector<std::function<void(Player&)>> chanceEvents;
    std::vector<std::function<void(Player&)>> communityChestEvents;
    Game();
    ~Game();
    void run();
    void initializeSDL();
    void createBoard();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void cleanup();
    void nextTurn();
    int rollDice();
    void buyHouse(Player& player, Tile& tile);
    void renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y);
};

#endif
