#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include "Tile.h"

class Player {
private:
    SDL_Texture* sprite;
    float x, y; // Vị trí hiện tại
public:
    // Constructor mặc định
    Player()
        : sprite(nullptr), x(0), y(0), previousRoll(0), currentRoll(0),
        money(1500), position(0), isMoving(false), targetX(0), targetY(0),
        canRollDice(true), isInJail(false), turnsInJail(0), jailTurns(0) {}

    // Constructor có tham số
    Player(const std::string& playerName, int initialMoney)
        : name(playerName), money(initialMoney), sprite(nullptr), x(0), y(0),
        previousRoll(0), currentRoll(0), position(0), isMoving(false),
        targetX(0), targetY(0), canRollDice(true), isInJail(false),
        turnsInJail(0), jailTurns(0) {}

    // Các phương thức công khai
    void addRoll(int roll);
    void printRollHistory() const;
    int calculateNewPosition(int steps) const;
    void move(int steps, std::vector<Tile>& board);
    void setTargetPosition(float x, float y);
    void updateTargetPosition();
    void updatePosition(float deltaTime, const std::vector<Player>& otherPlayers);
    bool canBuyHouse(const Tile& tile) const;
    void displayInfo() const;
    void addProperty(Tile* tile);
    int countHouses() const;
    void goToJail() {
        isInJail = true;
        turnsInJail = 3;  // Stay in jail for 3 turns by default
    }
    void releaseFromJail() {
        isInJail = false;
        turnsInJail = 0;
    }
    SDL_Texture* getSprite() const {
        return sprite;
    }
    void setSprite(SDL_Texture* texture) {
        sprite = texture;
    }
    float getX() const { return x; }
    float getY() const { return y; }

    // Các thành viên private
    std::string name;
    std::vector<int> rollHistory;
    int previousRoll;
    int currentRoll;
    int money;
    std::vector<Tile*> properties;
    int position;
    bool isMoving;
    float targetX, targetY; // vị trí mục tiêu
    bool canRollDice;
    bool isInJail;
    int turnsInJail;
    int jailTurns;  // Số lượt người chơi bị giam
    bool hasReachedTarget() const;
};

#endif
