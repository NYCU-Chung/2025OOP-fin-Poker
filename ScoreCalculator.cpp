#include "ScoreCalculator.h"
#include <map>
#include <algorithm>
#define conv(x) ((x) == 14 ? 11 : ((x) >= 10 ? 10 : (x)))
using namespace std;

/**
 *   辅助函数：判断五张牌在点数上是否真正连续（支持 A-2-3-4-5 情况）。
 *   要求 v.size() == 5，v 中的 Card 已经经过按点数从小到大排序过。
 */
static bool isTrueStraight(const vector<Card>& v) {
    if (v.size() != 5) return false;

    // 提取五张牌的点数到 vals
    vector<int> vals(5);
    for (int i = 0; i < 5; i++) {
        vals[i] = v[i].getValue();
    }
    // 正常按点数排序，这里再做一次保险
    sort(vals.begin(), vals.end());

    // —— 特殊情况：A2345
    // 如果点数排序后恰好是 {2,3,4,5,14}，也视为合法顺子
    if (vals == vector<int>{2,3,4,5,14}) {
        return true;
    }
    // —— 普通情况：检查是否每张都比前一张大 1
    for (int i = 1; i < 5; i++) {
        if (vals[i] != vals[i-1] + 1) return false;
    }
    return true;
}

string ScoreCalculator::check(const vector<Card>& _play) {
    vector<Card> play = _play;
    // 先按点数排序
    sort(play.begin(), play.end(), [](const Card &a, const Card &b) {
        return a.getValue() < b.getValue();
    });

    if (play.size() == 5) {
        // 先检测花色是否全部相同（用于 Flush / Straight Flush 判断）
        bool flush = true;
        for (int i = 1; i < 5; i++) {
            if (play[i].getSuit() != play[i-1].getSuit()) {
                flush = false;
                break;
            }
        }

        // 再检测点数是否连续或 A-2-3-4-5
        bool straight = isTrueStraight(play);

        if (flush && straight)      return "Straight Flush";
        else if (straight)          return "Straight";
        else if (flush)             return "Flush";
    }

    // 以下原有统计不同点数数量来判断其他牌型
    map<int,int> cnt;
    for (int i = 0; i < (int)play.size(); i++) {
        cnt[play[i].getValue()]++;
    }
    // Four of a Kind
    for (auto &kv : cnt) {
        if (kv.second == 4) return "Four of a Kind";
    }
    // Full House 检测
    bool three = false, two = false;
    for (auto &kv : cnt) {
        if (kv.second == 3) three = true;
        if (kv.second == 2) two = true;
    }
    if (three && two) return "Full House";
    if (three)        return "Three of a Kind";

    // Two Pair / Pair
    int pairCount = 0;
    for (auto &kv : cnt) {
        if (kv.second == 2) pairCount++;
    }
    if (pairCount == 2) return "Two Pair";
    if (pairCount == 1) return "Pair";

    // 默认 High Card
    return "High Card";
}

int ScoreCalculator::calculate(const vector<Card>& play) {
    if (play.empty()) return 0;

    string type = check(play);
    map<int,int> cnt;
    for (int i = 0; i < (int)play.size(); i++) {
        cnt[play[i].getValue()]++;
    }

    int score = 0;
    if (type == "Straight Flush") {
        // Straight Flush: 每张牌得分乘 9
        for (auto &c : play) {
            score += c.getScore() * 9;
        }
    }
    else if (type == "Four of a Kind") {
        // 找出那个四张相同点数的 value
        for (auto &kv : cnt) {
            if (kv.second == 4) {
                score = conv(kv.first) * 4 * 8;
                break;
            }
        }
    }
    else if (type == "Full House") {
        for (auto &c : play) {
            score += c.getScore() * 7;
        }
    }
    else if (type == "Flush") {
        for (auto &c : play) {
            score += c.getScore() * 6;
        }
    }
    else if (type == "Straight") {
        for (auto &c : play) {
            score += c.getScore() * 5;
        }
    }
    else if (type == "Three of a Kind") {
        for (auto &kv : cnt) {
            if (kv.second == 3) {
                score = conv(kv.first) * 3 * 4;
                break;
            }
        }
    }
    else if (type == "Two Pair") {
        for (auto &kv : cnt) {
            if (kv.second == 2) {
                score += conv(kv.first) * 2 * 3;
            }
        }
    }
    else if (type == "Pair") {
        for (auto &kv : cnt) {
            if (kv.second == 2) {
                score = conv(kv.first) * 2 * 2;
                break;
            }
        }
    }
    else if (type == "High Card") {
        int mx = 0;
        for (auto &c : play) {
            mx = max(mx, c.getScore());
        }
        score = mx;
    }
    return score;
}
