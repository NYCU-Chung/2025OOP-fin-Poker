#include "Card.h"
#include <sstream>
using namespace std;

Card::Card(Suit s, int v): suit(s), value(v) {}
Suit Card::getSuit() const { return suit; }
int Card::getValue() const { return value; }
int Card::getScore() const { return (value == 14 ? 11 : (value >= 10 ? 10 : value)); }
int Card::getScore(int v) const { return (v == 14 ? 11 : (v >= 10 ? 10 : v)); }
string Card::toString() const {
    static const char* names[] = {"C","D","H","S"};
    ostringstream os;
    os << names[(int)suit] << ":";
    if (value >= 2 && value <= 10) os << value;
    else if (value == 11) os << 'J';
    else if (value == 12) os << 'Q';
    else if (value == 13) os << 'K';
    else if (value == 14) os << 'A';
    return os.str();
}
