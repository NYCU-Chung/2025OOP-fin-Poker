#include <bits/stdc++.h>
#include "ScoreCalculator.h"
#include "Card.h"

// enum class Suit { Clubs, Diamonds, Hearts, Spades };
int main() {
    ScoreCalculator sc;
    vector<Card> v;
    v.push_back(Card((Suit)0, 6));
    v.push_back(Card((Suit)1, 3));
    v.push_back(Card((Suit)2, 7));
    v.push_back(Card((Suit)1, 9));
    v.push_back(Card((Suit)0, 5));
    cout << sc.check(v) << ' ' << sc.calculate(v) << '\n';
    return 0;
}