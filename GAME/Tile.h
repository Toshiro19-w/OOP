#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm> 

class Player;  // Forward declaration

enum class TileType {
    GO,
    NORMAL,
    START,
    JAIL,
    CHANCE,
    BEACH,
    COMMUNITY_CHEST,
    FREE_PARKING,
    GO_TO_JAIL,
    INCOME_TAX,
    LUXURY_TAX
};

// Cấu trúc ô cho cờ tỷ phú
struct Tile {
    std::string name;
    int x, y; // Tọa độ của ô trên màn hình
    int housePrice;
    int numHouses;
    int maxHouses;
    bool isOwned;
    TileType type;
    std::vector<Player*> playersOnTile;  // Lưu danh sách người chơi trên ô
    std::function<void(Player&)> event;  // Sự kiện xảy ra khi người chơi đến ô này

    // Constructor mặc định
    explicit Tile()
        : name(""), housePrice(0), numHouses(0), type(TileType::NORMAL), maxHouses(4), isOwned(false), event(nullptr), x(0), y(0) {}

    // Constructor tùy chỉnh (no default values here)
    explicit Tile(const std::string& name, int price, int numHouses, TileType t, int maxH, bool isOwned, std::function<void(Player&)> event, int xCoord, int yCoord)
        : name(name), housePrice(price), numHouses(numHouses), type(t), maxHouses(maxH), isOwned(isOwned), event(event), x(xCoord), y(yCoord) {}

    // Constructor with fewer parameters (with some default values)
    explicit Tile(const std::string& name, TileType t, int xCoord = 0, int yCoord = 0)
        : name(name), housePrice(0), numHouses(0), type(t), maxHouses(4), isOwned(false), event(nullptr), x(xCoord), y(yCoord) {}

    // Thêm người chơi vào ô
    void addPlayer(Player* player) {
        playersOnTile.push_back(player);
    }

    // Xóa người chơi khỏi ô
    void removePlayer(Player* player) {
        playersOnTile.erase(std::remove(playersOnTile.begin(), playersOnTile.end(), player), playersOnTile.end());
    }
};

#endif // TILE_H

