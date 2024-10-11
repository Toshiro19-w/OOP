#include "Game.h"

int main(int argc, char* argv[]) {
    std::vector<std::string> playerNames = { "Alice 1", "Bob 2", "Charlie 3", "Duck 4" };
    Game game(playerNames);
    game.run();
    return 0;
}