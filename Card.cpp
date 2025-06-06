#include "Card.h"
#include <sstream>
using namespace std;

Card::Card(Suit s, int v): suit(s), value(v) {}
Suit Card::getSuit() const { return suit; }
void Card::setSuit(Suit s) { suit = s; }
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
    const string emot = "  ฅ^•ﻌ•^ฅ  ";
    const int width = 11; // 包含邊框 +---------+
    const int innerW = width - 2;

    // 數值字串 (先存 raw_val，再決定是否要染色)
    string raw_val;
    if (value >= 2 && value <= 10) raw_val = to_string(value);
    else if (value == 11) raw_val = "J";
    else if (value == 12) raw_val = "Q";
    else if (value == 13) raw_val = "K";
    else if (value == 14) raw_val = "A";
    else raw_val = "?";

    // 帶顏色的數值顯示字串 (若是紅心或菱形，就套 ANSI 紅色碼)
    string display_val = raw_val;
    if (suit == Suit::Hearts || suit == Suit::Diamonds) {
        display_val = "\033[31m" + raw_val + "\033[0m";
    }

    // 花色符號 (紅心 & 菱形染紅，其餘保原色)
    static const char* suitsArr[] = {"♣", "♦", "♥", "♠"};
    string raw_symbol = suitsArr[(int)suit];
    string s_symbol = raw_symbol;
    if (suit == Suit::Diamonds || suit == Suit::Hearts) {
        s_symbol = "\033[31m" + raw_symbol + "\033[0m";
    }

    int symbol_display_len = 1; // 只顯示 1 個符號寬度，排版時就用這個

    vector<string> art;
    art.reserve(6);

    // Line 1: 表情
    art.push_back(emot);

    // Line 2: 上框
    art.push_back("+---------+");

    // Line 3: 左上角數值 (使用 raw_val.length() 算 padding，但顯示 display_val)
    {
        int raw_len = raw_val.length();
        string line = "|";
        line += display_val;
        line += string(innerW - raw_len, ' ');
        line += "|";
        art.push_back(line);
    }

    // Line 4: 花色置中
    {
        int totalPad = innerW - symbol_display_len;
        int leftPad = totalPad / 2;
        int rightPad = totalPad - leftPad;

        string line = "|";
        line += string(leftPad, ' ') + s_symbol + string(rightPad, ' ');
        line += "|";
        art.push_back(line);
    }

    // Line 5: 右下角數值 (同理，先算 raw_val.length()，顯示 display_val)
    {
        int raw_len = raw_val.length();
        string line = "|";
        line += string(innerW - raw_len, ' ') + display_val;
        line += "|";
        art.push_back(line);
    }

    // Line 6: 下框
    art.push_back("+---------+");

    return art;
}




/*vector<string> Card::getAsciiArt() const {
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

    if (suit == Suit::Diamonds || suit == Suit::Hearts) {
        s_symbol = string("") + "\033[31m" + s_symbol + "\033[0m";
    }


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
        art.push_back(string("|") + string(max(0, pad - 10), ' ') + val + string("|"));
    }

    // 第六行：同第二行
    art.push_back(string("+") + string(innerW-10, '-') + string("+"));

    return art;
}*/