#ifndef SCORECALC_H
#define SCORECALC_H
#include <vector>
#include "Card.h"
using namespace std;

class ScoreCalculator {
public:
    static int calculate(const vector<Card>& play);
    static string check(const vector<Card>& _play);
};
#endif // SCORECALC_H
