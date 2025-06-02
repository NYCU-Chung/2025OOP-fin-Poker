#ifndef GAME_H
#define GAME_H
#include "Player.h"
#include "include/json.hpp"

using json = nlohmann::json;

class Game {
public:
    void start();
private:
    json data;
    void settingStage();
    void playStage(Player &p);
    void ShowLeaderBoard();
    void settlementStage(Player &p);
    void writeData();
};
#endif // GAME_H
