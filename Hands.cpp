#include "Hands.h"
#include <algorithm>
using namespace std;

void Hand::add(Card c) { cards.push_back(c); }
void Hand::removeAt(int idx) { cards.erase(cards.begin() + idx); }
vector<Card>& Hand::getCards() { return cards; }
void Hand::sortByValue() {
    sort(cards.begin(), cards.end(), [](const Card &a, const Card &b){
        return a.getValue() < b.getValue();
    });
}
void Hand::sortBySuit() {
    sort(cards.begin(), cards.end(), [](const Card &a, const Card &b){
        return a.getSuit() < b.getSuit();
    });
}
void Hand::clear() {
    cards.clear();
}
