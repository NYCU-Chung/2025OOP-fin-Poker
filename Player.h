// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <map>
#include <vector>
#include "Hands.h"

using namespace std;

class Player {
public:
    Player();
    Player(const string &n);
    Player(const string &n, int initial_money);

    // ---------------- 玩家基本資訊 ----------------
    const string& name() const;

    // ---------------- 金錢／庫存 ----------------
    int  getMoney() const;
    void setMoney(int m);
    void addToInventory(const string &ticket);
    vector<string> getInventory() const;
    void removeFromInventory(const string &ticket); 
      // <— 新增：使用完道具後，要把它從 inventory 中消耗掉！

    // ---------------- 手牌相關 ----------------
    Hand& hand();

    // ---------------- 本局統計 ----------------
    const int score() const;
    const int play() const;
    const int discard() const;
    void resetStats();
    void recordPlay();
    void recordScore(int sc);
    void recordDiscard();
    void recordPlayType(const vector<Card>& play);
    void printStats() const;

private:
    string playerName;

    // 手牌
    Hand handCards;

    // 本局統計
    int totalPlayed    = 0;
    int totalDiscarded = 0;
    int totalScore     = 0;
    map<string,int> playTypes;

    // 「庫存」與「金錢」
    int money = 0;
    vector<string> inventory;
};

#endif // PLAYER_H
