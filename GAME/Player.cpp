#include "Player.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

Player::Player() : money(1500), position(0), isMoving(false), x(0), y(0), targetX(0), targetY(0), canRollDice(true), isInJail(false), turnsInJail(0), previousRoll(0), currentRoll(0), jailTurns(0) {}

void Player::addRoll(int roll) {
    rollHistory.push_back(roll);
}

bool Player::canBuyHouse(Tile& tile) {
    // Kiểm tra nếu người chơi sở hữu ô đất và ô đất chưa có số lượng nhà tối đa
    return find(properties.begin(), properties.end(), &tile) != properties.end() && tile.numHouses < tile.maxHouses;
}


void Player::printRollHistory() const {
    for (int roll : rollHistory) {
        std::cout << roll << " ";
    }
    std::cout << std::endl;
}

int Player::calculateNewPosition(int steps) const {
    int newPosition = (position + steps + 40) % 40; // NUM_TILES = 40
    return newPosition;
}

void Player::move(int steps, const std::vector<Tile>& board) {

    if (canRollDice) { // Kiểm tra nếu người chơi có thể tung xúc xắc
        if (isInJail) { // Kiểm tra nếu người chơi đang ở trong tù
            std::cout << "You are in jail! ";
            if (turnsInJail > 0) {
                std::cout << "Skipping this turn." << std::endl;
                turnsInJail--;  // Giảm số lượt trong tù
                canRollDice = false; // Người chơi đã hoàn thành lượt, không cho phép tung xúc xắc nữa
                return;
            }
            else {
                std::cout << "You are released from jail!" << std::endl;
                isInJail = false;  // Ra khỏi tù
                turnsInJail = 0;   // Đặt lại số lượt trong tù
                // Sau khi ra khỏi tù, người chơi được phép di chuyển
            }
        }

        // Nếu người chơi không ở trong tù hoặc đã được thả, họ di chuyển
        int newPosition = calculateNewPosition(steps);
        position = newPosition;
        updateTargetPosition();
        isMoving = true;
        canRollDice = false;  // Chặn người chơi tung xúc xắc thêm

        // Kích hoạt sự kiện ở ô mới nếu có
        if (board[position].event) {
            board[position].event(*this);
        }
    }
}

void Player::updateTargetPosition() {
    if (position < 10) {
        targetX = position * TILE_SIZE;
        targetY = 0;
    }
    else if (position < 20) {
        targetX = SCREEN_WIDTH - TILE_SIZE;
        targetY = (position - 10) * TILE_SIZE;
    }
    else if (position < 30) {
        targetX = SCREEN_WIDTH - (position - 20) * TILE_SIZE - TILE_SIZE;
        targetY = SCREEN_HEIGHT - TILE_SIZE;
    }
    else {
        targetX = 0;
        targetY = SCREEN_HEIGHT - (position - 30) * TILE_SIZE - TILE_SIZE;
    }
}

void Player::updatePosition(float deltaTime) {
    if (isMoving) {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < PLAYER_SPEED * deltaTime) {
            x = targetX;
            y = targetY;
            isMoving = false;
            canRollDice = true;
        }
        else {
            x += dx / distance * PLAYER_SPEED * deltaTime;
            y += dy / distance * PLAYER_SPEED * deltaTime;
        }
    }
}
