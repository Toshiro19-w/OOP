#ifndef BOARD_H
#define BOARD_H

#include "Player.h"
#include "Constants.h"
#include "Game.h"
#include "Tile.h"
#include <SDL.h>
#include <vector>

class Game;

class Board {
public:
    Board(Game *gameInstance);
    void createBoard();
    void calculateTilePosition(int i, int& x, int& y);

    // Các hàm tạo ra texture
    void renderPlayerAt(std::shared_ptr<Player> player, int x, int y);
    void renderPlayers();
    void renderBoards();

    // Xử lý các sự kiện
    void handleChanceEvent(const std::shared_ptr<Player>& player);
    void handleWorldsEvent(const std::shared_ptr<Player>& player);

    std::vector<Tile> board;
    Game* gameInstance;

private:
    // Tạo ra các ô đặc biệt trên bàn cờ
    void setupSpecialTiles();

    // Các phương thức mua nhà
    void applyTax(Player& player, Tile& tile);

    // Các hàm tạo ra texture
    void renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y);
};

#endif