#include "Game.h"
#include "ScoreCalculator.h"
#include <bits/stdc++.h>

using namespace std;

void Game::start() {
    ifstream fin("players.json");
    data = json::parse(fin);
    while(true) settingStage();
}

void Game::settingStage() {
    cout << "1. 登入\n2. 查看排行榜\n3. 登出\n4. 結束遊戲\n選擇: ";
    int choice; cin >> choice;
    static Player p;
    if (choice == 1) {
        cout << "輸入玩家名稱: ";
        string name; cin >> name;
        p = Player(name);
        playStage(p);
    } else if (choice == 2) {
        ShowLeaderBoard();
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
    while (playRounds < 4 && !p.hand().getCards().empty()) {
        cout << "手牌: ";
        for (auto &c : p.hand().getCards()) cout << c.toString() << " ";
        cout << "\n遊玩:1 出牌 2 棄牌 3 排序 4 結束回合\n選擇: ";
        int c; cin >> c;
        if (c == 1 && playRounds < 4) {
            int k; cout << "出幾張(1-5): "; cin >> k;
            vector<Card> play;
            for (int i = 0; i < k; i++) {
                play.push_back(p.hand().getCards().back());
                p.hand().removeAt(p.hand().getCards().size() - 1);
            }
            int sc = ScoreCalculator::calculate(play);
            cout << "本次得分: " << sc << "\n";
            p.recordPlay(k);
            p.recordScore(sc);
            p.recordPlayType(play);
            playRounds++;
        } else if (c == 2 && discardRounds < 3) {
            int k; cout << "棄幾張(1-5): "; cin >> k;
            for (int i = 0; i < k; i++) {
                p.hand().removeAt(p.hand().getCards().size() - 1);
            }
            p.recordDiscard(k);
            discardRounds++;
        } else if (c == 3) {
            cout << "排序:1 點數 2 花色: "; int m; cin >> m;
            if (m == 1) p.hand().sortByValue(); else p.hand().sortBySuit();
        } else break;
    }
    settlementStage(p);
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