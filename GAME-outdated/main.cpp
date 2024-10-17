#include "Game.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "============================= HOW TO PLAY ===============================\n";
    cout << "SPACE. do xuc xac\n";
    cout << "b. mua bai bien\n";
    cout << "i. hien thong tin cua nguoi choi hien tai\n";
    cout << "h. mua nha\n";
    cout << "s. ban nha\n";
    cout << "=========================================================================\n";
    std::vector<std::string> playerNames = { "Alice 1", "Bob 2", "Charlie 3", "Duck 4" };
    Game game(playerNames);
    game.run();
    return 0;
}