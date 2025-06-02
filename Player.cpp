#include "Player.h"
#include "ScoreCalculator.h"
#include <iostream>
using namespace std;

Player::Player(): totalDiscarded(3), totalPlayed(4) {}
Player::Player(const string &n): playerName(n), totalDiscarded(3), totalPlayed(4) {}
const string& Player::name() const { return playerName; }
const int Player::score() const { return totalScore; }
const int Player::play() const { return totalPlayed; }
const int Player::discard() const { return totalDiscarded; }
Hand& Player::hand() { return handCards; }
void Player::resetStats() { totalPlayed = totalDiscarded = 0; playTypes.clear(); }
void Player::recordPlay() { totalPlayed++; }
void Player::recordScore(int sc) { totalScore += sc; }
void Player::recordDiscard() { totalDiscarded++; }
void Player::recordPlayType(const vector<Card>& play) {
    string score = ScoreCalculator::check(play);
    playTypes[score]++;
}
void Player::printStats() const {
    cout << "總分: " << totalScore << "\n";
}
