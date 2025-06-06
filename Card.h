#ifndef CARD_H
#define CARD_H
#include <string>
#include <vector>
using namespace std;

enum class Suit { Clubs, Diamonds, Hearts, Spades };

class Card {
public:
    Card(Suit s, int v);
    Suit getSuit() const;
    void setSuit(Suit s);
    int getValue() const;
    int getScore() const;
    int getScore(int v) const;
    string toString() const;
    
    vector<string> getAsciiArt() const;
private:
    Suit suit;
    int value; // 2..10, 11=J,12=Q,13=K,14=A
};
#endif // CARD_H
