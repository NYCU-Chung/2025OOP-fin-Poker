#include "ScoreCalculator.h"
#include <map>
#include <algorithm>
#define conv(x) ((x) == 14 ? 11 : ((x) >= 10 ? 10 : (x)))
using namespace std;

string ScoreCalculator::check(const vector<Card>& _play) {
    vector<Card> play = _play;
    sort(play.begin(), play.end(), [](Card a, Card b) {
        return a.getValue() < b.getValue();
    });
    if(play.size() == 5) {
        bool ok = 1;
        for(int i=1;i<4;i++) if(play[i].getValue() != play[i-1].getValue() + 1) ok = 0;
        bool ok2 = 1;
        for(int i=1;i<4;i++) if(play[i].getSuit() != play[i-1].getSuit()) ok2 = 0;
        if(ok && ok2) return "Straight Flush";
        if(ok) return "Straight";
        if(ok2) return "Flush";
    }
    map<int,int> cnt;
    for(int i=0;i<play.size();i++) cnt[play[i].getValue()]++;
    for(auto& kv : cnt) if(kv.second == 4) return "Four of a Kind";
    bool ok3 = 0, ok4 = 0;
    for(auto& kv : cnt) {
        if(kv.second == 3) ok3 = 1;
        if(kv.second == 2) ok4 = 1;
    }
    if(ok3 && ok4) return "Full House";
    for(auto& kv : cnt) if(kv.second == 3) return "Three of a Kind";
    int c = 0;
    for(auto& kv : cnt) if(kv.second == 2) c++;
    if(c == 2) return "Two Pair";
    if(c == 1) return "Pair";
    return "High Card";
}

int ScoreCalculator::calculate(const vector<Card>& play) {
    if (play.empty()) return 0;
    int score = 0;
    string type = check(play);
    map<int,int> cnt;
    for(int i=0;i<play.size();i++) cnt[play[i].getValue()]++;
    if(type == "Straight Flush") for(int i=0;i<play.size();i++) score += play[i].getScore() * 9;
    if(type == "Four of a Kind") for(auto& kv : cnt) if(kv.second == 4) score = conv(kv.first)*4*8;
    if(type == "Full House") for(int i=0;i<play.size();i++) score += play[i].getScore() * 7;
    if(type == "Flush") for(int i=0;i<play.size();i++) score += play[i].getScore() * 6;
    if(type == "Straight") for(int i=0;i<play.size();i++) score += play[i].getScore() * 5;
    if(type == "Three of a Kind") for(auto& kv : cnt) if(kv.second == 3) score = conv(kv.first)*3*4;
    if(type == "Two Pair") for(auto& kv : cnt) if(kv.second == 2) score += conv(kv.first)*2*3;
    if(type == "Pair") for(auto& kv : cnt) if(kv.second == 2) score = conv(kv.first)*2*2;
    if(type == "High Card") {
        int mx = 0;
        for(int i=0;i<play.size();i++) mx = max(mx, play[i].getScore());
        score = mx;
    }
    return score;
}
