#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <memory>
#include "Tile.h"
#include "Board.h"
#include "Game.h"

enum class PlayerState { Normal, Bankrupt };

class Game; // Forward declaration cho lớp Game

class Player : public std::enable_shared_from_this<Player> {
public:
    // Constructor có tham số
    Player(const std::string& playerName, int initialMoney, Game* game); // Thay đổi kiểu đối số

    // Getters và setters
    const std::string& getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }

    std::shared_ptr<Game> getGameInstance() const { return gameInstance; }
    void setGameInstance(std::shared_ptr<Game> gameInstance) { this->gameInstance = gameInstance; }

    float getX() const { return x; }
    void setX(float x) { this->x = x; }

    float getY() const { return y; }
    void setY(float y) { this->y = y; }

    float getTargetX() const { return targetX; }
    void setTargetX(float targetX) { this->targetX = targetX; }

    float getTargetY() const { return targetY; }
    void setTargetY(float targetY) { this->targetY = targetY; }

    float getSpeed() const { return speed; }
    void setSpeed(float speed) { this->speed = speed; }

    std::vector<int> getRollHistory() const { return rollHistory; }
    void setRollHistory(const std::vector<int>& rollHistory) { this->rollHistory = rollHistory; }

    int getPreviousRoll() const { return previousRoll; }
    void setPreviousRoll(int previousRoll) { this->previousRoll = previousRoll; }

    int getCurrentRoll() const { return currentRoll; }
    void setCurrentRoll(int currentRoll) { this->currentRoll = currentRoll; }

    int getMoney() const { return money; }
    void setMoney(int money) { this->money = money; }

    std::vector<Tile*> getProperties() const { return properties; }
    void setProperties(const std::vector<Tile*>& properties) { this->properties = properties; }

    int getPosition() const { return position; }
    void setPosition(int position) { this->position = position; }

    PlayerState getState() const { return state; }
    void setState(PlayerState state) { this->state = state; }

    bool getIsMoving() const { return isMoving; }
    void setIsMoving(bool isMoving) { this->isMoving = isMoving; }

    bool getCanRollDice() const { return canRollDice; }
    void setCanRollDice(bool canRollDice) { this->canRollDice = canRollDice; }

    int getTurnsOnLostIsland() const { return turnsOnLostIsland; }
    void setTurnsOnLostIsland(int turnsOnLostIsland) { this->turnsOnLostIsland = turnsOnLostIsland; }

    bool getIsOnLostIsland() const { return isOnLostIsland; }
    void setIsOnLostIsland(bool isOnLostIsland) { this->isOnLostIsland = isOnLostIsland; }

    int getWorldsUsed() const { return worldsUsed; }
    void setWorldsUsed(int worldsUsed) { this->worldsUsed = worldsUsed; }

    bool getOnWorldTour() const { return onWorldTour; }
    void setOnWorldTour(bool onWorldTour) { this->onWorldTour = onWorldTour; }

    void setSprite(SDL_Texture* newSprite) {
        sprite = newSprite;
    }
    SDL_Texture* getSprite() const {
        return sprite;
    }

    // Các phương thức công khai
    void addRoll(int roll);
    void printRollHistory() const;
    int calculateNewPosition(int steps) const;
    void move(int steps, std::vector<Tile>& board);
    void setTargetPosition(float x, float y);
    bool canBuyHouse(const Tile& tile) const;
    void displayInfo() const;
    void addProperty(Tile* tile);
    int countHouses() const;
    void updatePosition(float deltaTime, const std::vector<std::shared_ptr<Player>>& otherPlayers);
    void updateTargetPosition();
    std::vector<Tile*> getOwnedProperties() const;
    bool hasReachedTarget() const { return !isMoving; } // Trả về true nếu không còn di chuyển

private:
    //Khai báo biến private
    SDL_Texture* sprite;
    float x, y; // Vị trí hiện tại
    float targetX, targetY; // Vị trí mục tiêu
    float speed; // Tốc độ di chuyển
    std::string name;
    std::vector<int> rollHistory;
    int previousRoll = 0;
    int currentRoll = 0;
    int money = 1500;
    std::vector<Tile*> properties;
    int position = 0;
    PlayerState state = PlayerState::Normal;        // Trạng thái của người chơi
    bool isMoving = false;  // Kiểm tra xem người chơi có đang di chuyển không
    bool canRollDice = true; // Kiểm tra xem người chơi có thể lăn xúc xắc không
    int turnsOnLostIsland = 0; // Số lượt trên Lost Island
    bool isOnLostIsland = false;  // Người chơi có đang ở trên Lost Island không
    int worldsUsed = 0;
    bool onWorldTour = false;
    std::shared_ptr<Game> gameInstance;
};

#endif