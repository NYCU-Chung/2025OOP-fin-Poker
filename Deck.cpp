#include "Deck.h"
#include <algorithm>
#include <random>
using namespace std;

Deck::Deck() {
    for (int s = 0; s < 4; ++s)
        for (int v = 2; v <= 14; ++v)
            cards.emplace_back((Suit)s, v);
}
void Deck::shuffle() {
    static mt19937 rng(random_device{}());
    std::shuffle(cards.begin(), cards.end(), rng);
}
Card Deck::draw() {
    if(empty()) Deck();
    Card c = cards.back();
    cards.pop_back();
    return c;
}
bool Deck::empty() const { return cards.empty(); }
