#ifndef HAND_H
#define HAND_H
#include <vector>
#include "Card.h"
using namespace std;

class Hand {
public:
    void add(Card c);
    void removeAt(int idx);
    vector<Card>& getCards();
    void sortByValue();
    void sortBySuit();
private:
    vector<Card> cards;
};
#endif // HAND_H
