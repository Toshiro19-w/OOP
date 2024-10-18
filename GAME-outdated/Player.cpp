#include "Player.h"
#include "Constants.h"
#include "Game.h"
#include "Board.h"
#include <iostream>
#include <cmath>

// Constructor có tham số
Player::Player(const std::string& playerName, int initialMoney, Game* game)
    : name(playerName), money(initialMoney), gameInstance(game)
    , sprite(nullptr), x(0), y(0), targetX(0), targetY(0), speed(PLAYER_SPEED)
    , previousRoll(0), currentRoll(0), position(0), state(PlayerState::Normal)
    , isMoving(false), canRollDice(true), turnsOnLostIsland(0), isOnLostIsland(false)
    , worldsUsed(0), onWorldTour(false) {}

void Player::addRoll(int roll) {
    rollHistory.push_back(roll);
}

void Player::addProperty(Tile* tile) {
    properties.push_back(tile);
}

int Player::countHouses() const {
    int totalHouses = 0;
    for (const auto& property : properties) {
        totalHouses += property->getNumHouses();
    }
    return totalHouses;
}

bool Player::canBuyHouse(const Tile& tile) const {
    return tile.getTileType() == TileType::PROPERTY && tile.getNumHouses() < MAX_HOUSE;
}

void Player::printRollHistory() const {
    for (int roll : rollHistory) {
        std::cout << roll << " ";
    }
    std::cout << std::endl;
}

int Player::calculateNewPosition(int steps) const {
    return (position + steps + NUM_TILES) % NUM_TILES;  // NUM_TILES needs to be updated correctly.
}

std::vector<Tile*> Player::getOwnedProperties() const {
    std::vector<Tile*> ownedProperties;
    for (Tile* tile : properties) { // Duyệt qua danh sách properties của người chơi
        if (tile->getOwnerName() == name) { // Kiểm tra nếu người chơi sở hữu ô đất
            ownedProperties.push_back(tile);
        }
    }
    return ownedProperties;
}

void Player::move(int steps, std::vector<Tile>& board) {
    if (canRollDice) {
        Tile& oldTile = board[position];
        oldTile.removePlayer(shared_from_this());

        if (onWorldTour) {
            // Hiển thị danh sách các ô đất có thể di chuyển đến
            std::cout << "Choose a tile to move to (tiles with owners are excluded):" << std::endl;
            for (int i = 0; i < NUM_TILES; ++i) {
                if (!board[i].getOwnerName().empty()) {
                    std::cout << i << ". " << board[i].getName() << std::endl;
                }
            }

            int choice;
            std::cin >> choice;

            // Xử lý lựa chọn
            if (choice >= 0 && choice < NUM_TILES && !board[choice].getOwnerName().empty()) {
                position = choice;
                onWorldTour = false;
                std::cout << "Moved to " << board[position].getName() << " using World Tour!" << std::endl;
            }
            else {
                std::cout << "Invalid choice." << std::endl;
                // Có thể thêm xử lý lỗi ở đây, ví dụ: yêu cầu người chơi chọn lại
            }
        }
        else {
            // Logic di chuyển bình thường
            position = calculateNewPosition(steps);
        }

        Tile& targetTile = board[position];
        setTargetPosition(targetTile.getX(), targetTile.getY());
        targetTile.addPlayer(shared_from_this());

        if (targetTile.getOnLand()) {
            std::shared_ptr<Player> currentPlayer = gameInstance->getPlayers()[gameInstance->getCurrentPlayerIndex()]; // Lấy currentPlayer
            targetTile.getOnLand()(currentPlayer); // Gọi onLand và truyền currentPlayer vào
        }

        canRollDice = false;
    }
}

void Player::displayInfo() const {
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Player: " << name << std::endl;
    std::cout << "Money: $" << money << std::endl;
    std::cout << "Position on board: " << position << std::endl;
    std::cout << "Number of houses owned: " << countHouses() << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

void Player::updateTargetPosition() {
    // Cập nhật tọa độ mục tiêu dựa trên vị trí
    if (position < 8) {  // Bottom row (left to right)
        targetX = position * TILE_SIZE;
        targetY = SCREEN_HEIGHT - TILE_SIZE;
    }
    else if (position < 16) {  // Right column (bottom to top)
        targetX = SCREEN_WIDTH - TILE_SIZE;
        targetY = SCREEN_HEIGHT - (position - 8) * TILE_SIZE;
    }
    else if (position < 24) {  // Top row (right to left)
        targetX = SCREEN_WIDTH - (position - 16) * TILE_SIZE;
        targetY = 0;
    }
    else if (position < 32) {  // Left column (top to bottom)
        targetX = 0;
        targetY = (position - 24) * TILE_SIZE;
    }
}

void Player::setTargetPosition(float x, float y) {
    targetX = x; // Gán giá trị x cho targetX
    targetY = y; // Gán giá trị y cho targetY
    isMoving = true; // Bắt đầu di chuyển khi có vị trí mục tiêu mới
}

void Player::updatePosition(float deltaTime, const std::vector<std::shared_ptr<Player>>& otherPlayers) {
    if (isMoving) {
        float targetSpeed = PLAYER_SPEED * deltaTime;
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= targetSpeed) {
            // Nếu gần đến vị trí mục tiêu, đặt vị trí vào tọa độ mục tiêu
            x = targetX;
            y = targetY;
            isMoving = false; // Dừng di chuyển
            canRollDice = true; // Bây giờ có thể lăn xúc xắc
        }
        else {
            // Di chuyển người chơi theo hướng mục tiêu với tốc độ
            x += (dx / distance) * targetSpeed;
            y += (dy / distance) * targetSpeed;
        }
    }
}

