#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Constants.h"
#include "Board.h"
#include "Tile.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <functional>

class Board;

class Game : std::enable_shared_from_this<Game> {
public:
    Game(const std::vector<std::string>& playerNames);  // Constructor
    ~Game();

    void run();
    void initializeSDL();
    void initPlayers();
    void cleanup();
    void quitGame();

    void handleEvents();
    void handleKeyPress(SDL_Keycode key);
    void update(float deltaTime);

    void render();
    void renderText(const std::string& message, int x, int y, SDL_Color color);
    void drawPlayer(SDL_Texture* texture, int x, int y);

    int rollDice();
    void handleRollDice(std::shared_ptr<Player> currentPlayer);
    void nextTurn();  // Switch to the next player's turn

    bool hasRolledDoubles(); // Nổ đôi giống nhau
    void setupChanceEvents();

    void buyHouse(Tile& tile);
    void buyBeach(std::shared_ptr<Player>, Tile& tile);

    void sellHouse(std::shared_ptr<Player> player);
    std::vector<Tile*> getOwnedTilesWithHouses(std::shared_ptr<Player> player);
    void displayOwnedTiles(std::shared_ptr<Player> player, const std::vector<Tile*>& ownedTiles);
    int getPlayerChoice(size_t numChoices);
    void sellHouseOnTile(std::shared_ptr<Player> player, Tile* tile);

    SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);

    // Getters
    std::vector<std::shared_ptr<Player>>& getPlayers() { return players; }
    std::unique_ptr<Board>& getBoard() { return board; }
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }
    bool getIsRunning() const { return isRunning; }
    SDL_Renderer* getRenderer() const { return renderer; }
    SDL_Texture* getHouseTexture() const { return houseTexture; }
    const std::vector<std::function<void(Player&)>>& getChanceEvents() const {
        return chanceEvents;
    }

    // Setters
    void setCurrentPlayerIndex(int currentPlayerIndex) { this->currentPlayerIndex = currentPlayerIndex; }
    void setIsRunning(bool isRunning) { this->isRunning = isRunning; }
    void setRenderer(SDL_Renderer* renderer) { this->renderer = renderer; }
    void setChanceEvents(const std::vector<std::function<void(Player&)>>& newEvents) {
        this->chanceEvents = newEvents;
    }

private:
    TTF_Font* font;
    SDL_Texture* houseTexture;
    std::vector<std::shared_ptr<Player>> players;
    std::unique_ptr<Board> board;
    int currentPlayerIndex;
    bool isRunning;

    std::vector<std::function<void(Player&)>> chanceEvents;

    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif  // GAME_H