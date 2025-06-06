#include "Game.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(time(NULL));
    Game g;
    g.start();
    return 0;
}
