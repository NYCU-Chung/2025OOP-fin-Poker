#include "Player.h"
#include "ScoreCalculator.h"
#include <iostream>
using namespace std;

Player::Player() {}
Player::Player(const string &n): playerName(n) {}
const string& Player::name() const { return playerName; }
const int Player::score() const { return totalScore; }
Hand& Player::hand() { return handCards; }
void Player::resetStats() { totalPlayed = totalDiscarded = 0; playTypes.clear(); }
void Player::recordPlay(int count) { totalPlayed += count; }
void Player::recordScore(int sc) { totalScore += sc; }
void Player::recordDiscard(int count) { totalDiscarded += count; }
void Player::recordPlayType(const vector<Card>& play) {
    string score = ScoreCalculator::check(play);
    playTypes[score]++;
}
void Player::printStats() const {
    cout << "總出牌張數: " << totalPlayed << "\n";
    for (auto &kv: playTypes)
        cout << "牌型 " << kv.first << " 的出現次數: " << kv.second << "\n";
    cout << "總棄牌張數: " << totalDiscarded << "\n";
}
