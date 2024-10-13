#ifndef BOARD_H
#define BOARD_H

#include "Player.h"
#include "Constants.h"
#include "Game.h"
#include "Tile.h"
#include <SDL.h>
#include <vector>
#include <functional>

class Game;

class Board {
public:
    Board(Game* gameInstance); // Constructor 

    // Tạo ra bảng
    void createBoard(); 

    // hàm hỗ trợ
    Tile createTile(const std::string& name, int cost, TileType type, int houseMax, bool owned, std::function<void(Player&)> onLand, int x, int y);
    
    // Tạo ra các ô trên bàn cờ
    void setupTilePositions();

    // Tạo ra các ô đặc biệt trên bàn cờ
    void setupSpecialTiles();

    // Đưa người chơi vào tù
    void sendPlayerToJail(Player& player);

    // Các phương thức mua nhà
    void buyHouse(Player& player, Tile& tile);
    void buyBeach(Player& player, Tile& tile); 
    void sellHouse(Player& player, Tile& tile);
    void applyTax(Player& player, Tile& tile);

    // Các hàm tạo ra texture
    void renderHouse(SDL_Renderer* renderer, SDL_Texture* houseTexture, int x, int y);
    void renderPlayerAt(Player* player, int x, int y);
    void renderPlayers();
    void renderBoards();

    // Rút ngẫu nhiên 1 lá cơ hội hoặc khí vận nếu người chơi vào ô cơ hội hoặc khí vận
    void handleChanceEvent(Player& player);
    void handleComChestEvent(Player& player);

    std::vector<Tile> board;
private:
    Game* gameInstance;
    std::vector<Tile> tiles;
    std::vector<std::function<void(Player&)>> chanceEvents;
    std::vector<std::function<void(Player&)>> communityChestEvents;
};

#endif
