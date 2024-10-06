#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <string>
#include <functional>

class Player; // Forward declaration

// Cấu trúc ô cho cờ tỷ phú
struct Tile {
    std::string name;
    int cost;
    std::string type; // "đất", "bắt đầu", "nhà tù", "bãi đậu xe", "cơ hội", "khí vận"
    std::function<void(Player&)> event; // Hàm để tạo ra các sự kiện
    int numHouses;
    int maxHouses;
    int housePrice;
};

#endif
