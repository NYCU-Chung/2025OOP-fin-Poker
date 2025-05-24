#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <map>
#include "Hands.h"
using namespace std;

class Player {
public:
    Player();
    Player(const string &n);
    const string& name() const;
    const int score() const;
    Hand& hand();
    void resetStats();
    void recordPlay(int count);
    void recordScore(int sc);
    void recordDiscard(int count);
    void recordPlayType(const vector<Card>& play);
    void printStats() const;
private:
    string playerName;
    Hand handCards;
    int totalPlayed = 0;
    int totalDiscarded = 0;
    int totalScore = 0;
    map<string,int> playTypes;
};
#endif // PLAYER_H
