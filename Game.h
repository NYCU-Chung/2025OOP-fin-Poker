#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Shop.h"
#include "include/json.hpp"
#include <vector>
#include <string>

using json = nlohmann::json;

class Game {
public:
    Game();
    void start();            // 遊戲啟動進入主迴圈

private:
    json data;               // 讀取 / 寫回 players.json
    std::vector<Player> players;
    Shop shop;               // 商店

    void loadPlayers();      // 由 data 讀出所有玩家到 vector<Player>
    void writeData();        // 把 JSON dump 回檔案

    void showMainMenu();     // 顯示「登入／排行榜／結束遊戲」主選單
    void handleMainChoice(int choice);

    int  findPlayerIndex(const std::string &name);
    void loginAndPlay();     // 處理玩家登入、先進商店、再 playStage
    void showLeaderBoard();  // 顯示排行榜
    void playStage(Player &p);
    void settlementStage(Player &p);
};

#endif // GAME_H
