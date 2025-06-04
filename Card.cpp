#include "Card.h"
#include <sstream>
using namespace std;

Card::Card(Suit s, int v): suit(s), value(v) {}
Suit Card::getSuit() const { return suit; }
int Card::getValue() const { return value; }
int Card::getScore() const { return (value == 14 ? 11 : (value >= 10 ? 10 : value)); }
int Card::getScore(int v) const { return (v == 14 ? 11 : (v >= 10 ? 10 : v)); }
string Card::toString() const {
    static const char* names[] = {"♣","♦","♥","♠"};
    ostringstream os;
    os << names[(int)suit];
    if (value >= 2 && value <= 10) os << value;
    else if (value == 11) os << 'J';
    else if (value == 12) os << 'Q';
    else if (value == 13) os << 'K';
    else if (value == 14) os << 'A';
    return os.str();
}
vector<string> Card::getAsciiArt() const {
    // 1. 新的表情
    const string emot = "ฅ^•ﻌ•^ฅ";
    int W = static_cast<int>(emot.length()); // 取 byte 長度作為總寬
    int innerW = W - 2;                      // 去掉兩側 '+' 後的內寬

    // 2. 將數值轉為字串
    string val;
    if (value >= 2 && value <= 10) {
        val = to_string(value);
    } else if (value == 11) {
        val = "J";
    } else if (value == 12) {
        val = "Q";
    } else if (value == 13) {
        val = "K";
    } else if (value == 14) {
        val = "A";
    } else {
        val = "?";
    }

    // 3. 花色符號
    static const char* suitsArr[] = {"♣", "♦", "♥", "♠"};
    string s_symbol = suitsArr[(int)suit];

    vector<string> art;
    art.reserve(6);

    // 第一行：emoticon
    art.push_back(emot);

    // 第二行：+---------+
    art.push_back(string("+") + string(innerW-10, '-') + string("+"));

    // 第三行：|V        |
    {
        int pad = innerW - static_cast<int>(val.length());
        art.push_back(string("|") + val + string(pad-10, ' ') + string("|"));
    }

    // 第四行：|    S    |
    {
        int leftPad  = (innerW - static_cast<int>(s_symbol.length())) / 2;
        int rightPad = innerW - static_cast<int>(s_symbol.length()) - leftPad;
        art.push_back(string("| ") + string(leftPad-5, ' ')
                             + s_symbol
                             + string(rightPad-5, ' ')
                             + string(" |"));
    }

    // 第五行：|        V|
    {
        int pad = innerW - static_cast<int>(val.length());
        art.push_back(string("|") + string(pad-10, ' ') + val + string("|"));
    }

    // 第六行：同第二行
    art.push_back(string("+") + string(innerW-10, '-') + string("+"));

    return art;
}