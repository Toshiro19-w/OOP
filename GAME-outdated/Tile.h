#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <memory> 

class Player;

// Enum representing the color group for properties
enum class ColorGroup {
    NONE,
    BROWN,
    LIGHT_BLUE,
    PINK,
    ORANGE,
    RED,
    YELLOW,
    GREEN,
    DARK_BLUE
};

// Enum representing the type of a tile
enum class TileType {
    GO,
    PROPERTY,
    START,
    CHANCE,
    BEACH,
    WORLDS,
    LOST_ISLAND,
    WORLD_TOUR,
    TAX
};

class Tile {
public:
    Tile() = default; // Constructor mặc định

    // Getters/Setters
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    int getX() const { return x; }
    int getY() const { return y; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }

    int getHousePrice() const { return housePrice; }
    void setHousePrice(int price) { housePrice = price; }

    int getNumHouses() const { return numHouses; }
    void setNumHouses(int count) { numHouses = count; }

    TileType getTileType() const { return type; }
    void setTileType(TileType newType) { type = newType; }

    std::string getOwnerName() const { return ownerName; }
    void setOwnerName(const std::string& newOwnerName) { ownerName = newOwnerName; }

    ColorGroup getColorGroup() const { return colorGroup; }
    void setColorGroup(ColorGroup group) { colorGroup = group; }

    int getValueMultiplier() const { return valueMultiplier; }
    void setValueMultiplier(int newValueMultiplier) { valueMultiplier = newValueMultiplier; }

    std::vector<std::shared_ptr<Player>> getPlayersOnTile() const { return playersOnTile; }
    void setPlayersOnTile(const std::vector<std::shared_ptr<Player>>& players) { playersOnTile = players; }

    std::function<void(const std::shared_ptr<Player>&)> getOnLand() const { return onLand; }
    void setOnLand(const std::function<void(const std::shared_ptr<Player>&)>& newOnLand) {  onLand = newOnLand; }

    // Các hàm khác
    void addPlayer(std::shared_ptr<Player> player) {
        playersOnTile.push_back(player);
    }

    void removePlayer(std::shared_ptr<Player> player) {
        auto it = std::remove(playersOnTile.begin(), playersOnTile.end(), player);
        playersOnTile.erase(it, playersOnTile.end());
    }

private:
    // Các thành viên private
    std::string name;
    std::string ownerName;
    int x, y;
    int housePrice;
    int numHouses;
    int maxHouses;
    int valueMultiplier;
    TileType type;
    ColorGroup colorGroup;
    std::vector<std::shared_ptr<Player>> playersOnTile;
    std::function<void(const std::shared_ptr<Player>&)> onLand; // Thay đổi kiểu dữ liệu của onLand

    // Constructor private chỉ được sử dụng bởi TileBuilder
    Tile(const std::string& name, int price, int numHouses, TileType t, int maxH,
        std::function<void(const std::shared_ptr<Player>&)> onLand, int xCoord, int yCoord, ColorGroup group,
        const std::string& ownerName)
        : name(name), ownerName(ownerName), x(xCoord), y(yCoord), housePrice(price), numHouses(numHouses), maxHouses(maxH),
        valueMultiplier(1), type(t), colorGroup(group), onLand(onLand) {}

    friend class TileBuilder; // Cho phép TileBuilder truy cập vào constructor private
};

#endif // TILE_H