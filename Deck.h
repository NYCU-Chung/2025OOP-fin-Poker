#ifndef DECK_H
#define DECK_H
#include <vector>
#include "Card.h"
using namespace std;

class Deck {
public:
    Deck();
    void shuffle();
    Card draw();
    bool empty() const;
private:
    vector<Card> cards;
};
#endif // DECK_H
