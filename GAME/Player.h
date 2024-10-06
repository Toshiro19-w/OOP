#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include "Tile.h"

class Player {
public:
    Player();
    
    // Các phương thức công khai
    void addRoll(int roll);
    void printRollHistory() const;
    int calculateNewPosition(int steps) const;
    void move(int steps, const std::vector<Tile>& board);
    void updateTargetPosition();
    void updatePosition(float deltaTime);
    bool canBuyHouse(Tile& tile);

    // Các phương thức getter và setter
    // int getCurrentRoll() const;
    // int getMoney() const;
    // void setMoney(int amount);
    // int getPosition() const;
    // void setPosition(int pos);
    // bool getIsMoving() const;
    // void setIsMoving(bool moving);
    // bool getCanRollDice() const;
    // void setCanRollDice(bool canRoll);
    // bool getIsInJail() const;
    // void setIsInJail(bool inJail);
    // int getTurnsInJail() const;
    // void setTurnsInJail(int turns);

//private:
    std::vector<int> rollHistory;
    int previousRoll;
    int currentRoll;
    int money;
    std::vector<Tile*> properties;
    int position;
    bool isMoving;
    float x, y; // Vị trí hiện tại
    float targetX, targetY; // vị trí mục tiêu
    bool canRollDice;
    bool isInJail;
    int turnsInJail;
    int jailTurns;  // Số lượt người chơi bị giam
};

#endif
