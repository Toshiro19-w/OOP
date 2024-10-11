#include "Player.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

void Player::addRoll(int roll) {
    rollHistory.push_back(roll);
}

void Player::addProperty(Tile* tile) {
    properties.push_back(tile);
}

int Player::countHouses() const {
    int totalHouses = 0;
    for (const auto& property : properties) {
        totalHouses += property->numHouses;
    }
    return totalHouses;
}

bool Player::canBuyHouse(const Tile& tile) const {
    return tile.type == NORMAL && tile.numHouses < tile.maxHouses;
}

void Player::printRollHistory() const {
    for (int roll : rollHistory) {
        std::cout << roll << " ";
    }
    std::cout << std::endl;
}

int Player::calculateNewPosition(int steps) const {
    return (position + steps + 40) % 40; // Bàn cờ có 40 ô
}

void Player::move(int steps, std::vector<Tile>& board){
    if (canRollDice) {
        if (board[position].type == GO_TO_JAIL) {
            position = 10; // Di chuyển thẳng vào tù
            updateTargetPosition();
            isInJail = true;
            turnsInJail = 3; // Người chơi sẽ bị kẹt trong tù 3 lượt
            canRollDice = false;
            isMoving = false;
        }

        if (isInJail) {
            std::cout << "--------------------------------" << std::endl;
            std::cout << "You are in jail! ";
            if (turnsInJail > 0) {
                std::cout << "Skipping this turn." << std::endl;
                canRollDice = false;
                return;
            }
            else {
                std::cout << name << " is released from jail!" << std::endl;
                isInJail = false;
                turnsInJail = 0;
            }
        }

        // Lấy ô hiện tại và loại bỏ người chơi khỏi ô đó
        Tile& oldTile = board[position];
        oldTile.removePlayer(this);

        // Di chuyển người chơi đến vị trí mới
        int newPosition = (position + steps) % board.size();
        position = newPosition;
        Tile& targetTile = board[newPosition];
        setTargetPosition(targetTile.x, targetTile.y);
        updateTargetPosition();
        isMoving = true;
        canRollDice = false;

        // Thêm người chơi vào ô mới
        Tile& newTile = board[position];
        newTile.addPlayer(this);

        // Kích hoạt sự kiện trên ô mới nếu có
        if (board[position].event) {
            board[position].event(*this);
        }
    }
}

void Player::displayInfo() const {
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Player: " << name << std::endl;
    std::cout << "Money: $" << money << std::endl;
    std::cout << "Position on board: " << position << std::endl;
    std::cout << "Number of houses owned: " << countHouses() << std::endl;
    std::cout << "In Jail: " << (isInJail ? "Yes" : "No") << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

void Player::setTargetPosition(float x, float y) {
    targetX = x;
    targetY = y;
    isMoving = true;
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

void Player::updatePosition(float deltaTime, const std::vector<Player>& otherPlayers) {
    if (isMoving) {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < PLAYER_SPEED * deltaTime) {
            x = targetX;
            y = targetY;
            isMoving = false;
            canRollDice = true;

            // Kiểm tra va chạm với người chơi khác và điều chỉnh vị trí nếu cần
            for (const Player& other : otherPlayers) {
                if (&other != this && other.x == x && other.y == y) {
                    x += TILE_SIZE * 0.1f;  // Điều chỉnh x để không bị trùng vị trí
                    y += TILE_SIZE * 0.1f;  // Điều chỉnh y để không bị trùng vị trí
                }
            }
        }
        else {
            x += dx / distance * PLAYER_SPEED * deltaTime;
            y += dy / distance * PLAYER_SPEED * deltaTime;
        }
    }
}

bool Player::hasReachedTarget() const {
    return (x == targetX && y == targetY);
}
