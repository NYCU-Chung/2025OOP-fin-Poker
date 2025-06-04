#include "Game.h"
#include "Deck.h"
#include "ScoreCalculator.h"
#include <bits/stdc++.h>

using namespace std;

static void printCardsInTwoRows(const vector<Card>& cards) {
    if (cards.size() < 8) {
        cerr << "printCardsInTwoRows 需要至少 8 張卡片\n";
        return;
    }

    // 先把前 8 張拆成上下各 4 張
    vector<Card> top4(cards.begin(), cards.begin() + 4);
    vector<Card> bot4(cards.begin() + 4, cards.begin() + 8);

    // 取得每張卡的 ASCII 畫板 (每張 6 行)
    vector<vector<string>> topArts, botArts;
    topArts.reserve(4);
    botArts.reserve(4);
    for (int i = 0; i < 4; i++) {
        topArts.push_back(top4[i].getAsciiArt());
        botArts.push_back(bot4[i].getAsciiArt());
    }

    // 假定每張卡的寬度都相同，都等於 art[0].length()
    int W = static_cast<int>(topArts[0][0].length());
    const int CARD_HEIGHT = 6; // getAsciiArt() 會回傳 6 行

    // ───── 上排 ASCII Art ─────
    for (int row = 0; row < CARD_HEIGHT; row++) {
        for (int c = 0; c < 4; c++) {
            cout << topArts[c][row] << "  ";
        }
        cout << "\n";
    }
    // 上排索引：0~3
    for (int c = 0; c < 4; c++) {
        string idxStr = "(" + to_string(c) + ")";
        int padLeft  = (W - static_cast<int>(idxStr.length())) / 2;
        int padRight = W - static_cast<int>(idxStr.length()) - padLeft;
        cout << string(padLeft-5, ' ')
             << idxStr
             << string(padRight-5, ' ')
             << "  ";
    }
    cout << "\n\n";

    // ───── 下排 ASCII Art ─────
    for (int row = 0; row < CARD_HEIGHT; row++) {
        for (int c = 0; c < 4; c++) {
            cout << botArts[c][row] << "  ";
        }
        cout << "\n";
    }
    // 下排索引：4~7
    for (int c = 0; c < 4; c++) {
        int idx = 4 + c;
        string idxStr = "(" + to_string(idx) + ")";
        int padLeft  = (W - static_cast<int>(idxStr.length())) / 2;
        int padRight = W - static_cast<int>(idxStr.length()) - padLeft;
        cout << string(padLeft-5, ' ')
             << idxStr
             << string(padRight-5, ' ')
             << "  ";
    }
    cout << "\n";
}


void Game::start() {
    ifstream fin("players.json");
    data = json::parse(fin);
    while(true) settingStage();
}

void Game::settingStage() {
    system("clear");
    cout << "1. 登入\n2. 查看排行榜\n3. 登出\n4. 結束遊戲\n選擇: ";
    int choice; cin >> choice;
    string s;
    getline(cin, s);
    static Player p;
    if (choice == 1) {
        cout << "輸入玩家名稱: ";
        string name; cin >> name;
        p = Player(name);
        playStage(p);
    } else if (choice == 2) {
        ShowLeaderBoard();
        cout << "按下 Enter 繼續..."; getline(cin, s);
    } else if (choice == 3) {
        for(auto &d : data) {
            if(d["name"] == p.name()) {
                d["maxScore"] = max((int)d["maxScore"], p.score());
                writeData();
                return;
            }
        }
        data.push_back({{"name", p.name()}, {"maxScore", p.score()}});
        writeData();
    } else {
        writeData();
        exit(0);
    }
}

void Game::playStage(Player &p) {
    Deck deck; deck.shuffle();
    p.resetStats();
    // 發手牌
    for (int i = 0; i < 8; i++) p.hand().add(deck.draw());
    int playRounds = 0, discardRounds = 0;
    string s;
    while (playRounds < 4 && !p.hand().getCards().empty()) {
        system("clear");
        cout << "================================\n";
        cout << "分數: " << p.score() << '\n';
        cout << "累計出牌次數: " << p.play() << '\n';
        cout << "累計棄牌次數: " << p.discard() << '\n';
        cout << "================================\n";
        
        // ─── 修改點：改用 ASCII Art 列印整副手牌 ───────────────────
        cout << "手牌:\n";
        vector<Card>& handCards = p.hand().getCards();
        if (handCards.size() >= 8) {
            printCardsInTwoRows(handCards);
        } else {
            // 若手牌未必有 8 張，可自行設計單排或其他顯示方式
            printCardsInTwoRows(handCards);
        }
        // ─────────────────────────────────────────────────────────

        cout << "\n(1)出牌 (2)棄牌 (3)排序 (4)結束回合\n選擇: ";
        int c; cin >> c;
        getline(cin, s);
        if (c == 1 && playRounds < 4) {
            cout << "輸入要出的牌的索引編號 (0-based): "; getline(cin, s);
            stringstream ss(s);
            vector<int> v;
            vector<Card> play;
            int id;
            while(ss >> id) {
                v.push_back(id);
                play.push_back(p.hand().getCards()[id]);
            }
            sort(v.begin(), v.end(), greater<int>());
            for(int id : v) {
                p.hand().removeAt(id);
            }
            cout << "本次牌型: " << ScoreCalculator::check(play) << "\n";
            int sc = ScoreCalculator::calculate(play);
            cout << "本次得分: " << sc << "\n";
            p.recordPlay();
            p.recordScore(sc);
            p.recordPlayType(play);
            playRounds++;
            for (int i = 7-v.size(); i >= 0; i--) p.hand().removeAt(i);
            for (int i = 0; i < 8; i++) p.hand().add(deck.draw());
            cout << "按下 Enter 繼續..."; getline(cin, s);
        } else if (c == 2) {
            if(discardRounds == 3) {
                cout << "已達到棄牌上限，按下 Enter 繼續..."; getline(cin, s);
                continue;
            }
            cout << "輸入要棄的牌的索引編號 (0-based): "; getline(cin, s);
            stringstream ss(s);
            vector<int> v;
            vector<Card> play;
            int id, k = 0;
            while(ss >> id) v.push_back(id);
            sort(v.begin(), v.end(), greater<int>());
            for(int id : v) {
                k++;
                p.hand().removeAt(id);
            }
            for (int i = 0; i < k; i++) {
                p.hand().add(deck.draw());
            }
            p.recordDiscard();
            discardRounds++;
        } else if (c == 3) {
            cout << "(1)依點數排序 (2)依花色排序:\n選擇: "; int m; cin >> m;
            if (m == 1) p.hand().sortByValue();
            else p.hand().sortBySuit();
        } else break;
    }
    settlementStage(p);
    cout << "按下 Enter 繼續..."; getline(cin, s);
}

void Game::ShowLeaderBoard() {
    vector<pair<int, string>> v;
    for(auto d : data) v.push_back({d["maxScore"], d["name"]});
    sort(v.begin(), v.end(), greater<pair<int, string>>());
    for(int i = 0; i < (int)v.size(); i++) cout << "Rank " << i+1 << ": " << v[i].second << ", Score : " << v[i].first << '\n';
}

void Game::settlementStage(Player &p) {
    cout << "--- 結算 ---\n";
    p.printStats();
}

void Game::writeData() {
    ofstream fout("players.json");
    fout << data.dump(2);
}