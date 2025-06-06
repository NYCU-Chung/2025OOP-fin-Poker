// Player.cpp
#include "Player.h"
#include "ScoreCalculator.h"
#include <algorithm>
#include <iostream>
using namespace std;

// ———— 建構子區 ————
Player::Player()
    : playerName(""), totalPlayed(0), totalDiscarded(0), totalScore(0), money(0)
{}

Player::Player(const string &n)
    : playerName(n), totalPlayed(0), totalDiscarded(0), totalScore(0), money(0)
{}

Player::Player(const string &n, int initial_money)
    : playerName(n), totalPlayed(0), totalDiscarded(0), totalScore(0), money(initial_money)
{}

// ———— Getter / Setter ————
const string& Player::name() const {
    return playerName;
}

int Player::getMoney() const {
    return money;
}

void Player::setMoney(int m) {
    money = m;
}

void Player::addToInventory(const string &ticket) {
    inventory.push_back(ticket);
}

vector<string> Player::getInventory() const {
    return inventory;
}

void Player::removeFromInventory(const string &ticket) {
    // 只移除第一個匹配的 ticket
    auto it = find(inventory.begin(), inventory.end(), ticket);
    if (it != inventory.end()) {
        inventory.erase(it);
    }
}

// ———— 手牌相關 ————
Hand& Player::hand() {
    return handCards;
}

// ———— 本局統計相關 ————
const int Player::score() const {
    return totalScore;
}

const int Player::play() const {
    return totalPlayed;
}

const int Player::discard() const {
    return totalDiscarded;
}

void Player::resetStats() {
    totalPlayed    = 0;
    totalDiscarded = 0;
    totalScore     = 0;
    playTypes.clear();
    // 注意：resetStats 不會重置 money / inventory，僅重置本局的出牌/棄牌/總分 與牌型紀錄
}

void Player::recordPlay() {
    totalPlayed++;
}

void Player::recordScore(int sc) {
    totalScore += sc;
}

void Player::recordDiscard() {
    totalDiscarded++;
}

void Player::recordPlayType(const vector<Card>& play) {
    string comboName = ScoreCalculator::check(play);
    playTypes[comboName]++;
}

void Player::printStats() const {
    cout << "===== 本局結算 =====\n";
    cout << "玩家: " << playerName << "\n";
    cout << "總分: " << totalScore << "\n";
    cout << "累計出牌次數: " << totalPlayed << "\n";
    cout << "累計棄牌次數: " << totalDiscarded << "\n";
    cout << "牌型分布:\n";
    for (const auto &entry : playTypes) {
        cout << "  " << entry.first << " : " << entry.second << "\n";
    }
    cout << "====================\n";
}
