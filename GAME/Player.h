#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include "Tile.h"

// Lớp người chơi
class Player {
public:
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

    Player();
    void addRoll(int roll);
    void printRollHistory() const;
    int calculateNewPosition(int steps) const;
    void move(int steps, const std::vector<Tile>& board);
    void updateTargetPosition();
    void updatePosition(float deltaTime);
    bool canBuyHouse(Tile& tile);
};

#endif