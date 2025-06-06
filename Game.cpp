// Game.cpp
#include "Game.h"
#include "Deck.h"
#include "ScoreCalculator.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#include <bits/stdc++.h>
using namespace std;

Game::Game()
    : shop(data)    // shop 用同一個 json reference
{}

void Game::start() {
    // 1. 先從檔案載入 data
    ifstream fin("players.json");
    if (!fin.is_open()) {
        // 如果沒有檔案，就先塞一個空的 array
        data = json::array();
    } else {
        data = json::parse(fin);
    }
    fin.close();

    // 2. 將 JSON 中的玩家解到 players vector（只讀 name + money）
    loadPlayers();

    // 3. 進入主選單迴圈
    while (true) {
        showMainMenu();
        int choice;
        cin >> choice;
        if (choice < 0 || choice > 2) continue;
        if (choice == 0) {
            // 離開遊戲前要把所有修改後的 JSON 存回去
            writeData();
            cout << "遊戲結束，謝謝遊玩！\n";
            exit(0);
        }
        handleMainChoice(choice);
    }
}

void Game::showMainMenu() {
    system("clear");
    cout << "=== Poker Game 主選單 ===\n";
    cout << "1. 登入並開始遊戲\n";
    cout << "2. 查看排行榜\n";
    cout << "0. 結束遊戲\n";
    cout << "請輸入選項：";
}

void Game::handleMainChoice(int choice) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清掉殘留的換行
    if (choice == 1) {
        loginAndPlay();
    } else if (choice == 2) {
        showLeaderBoard();
        cout << "按下 Enter 返回。\n";
        cin.get();
    }
}

// 由 data 讀到 players vector 中（只保 name + money）
void Game::loadPlayers() {
    players.clear();
    for (auto &entry : data) {
        std::string nm = entry["name"];
        int m  = (entry.contains("money") ? (int)entry["money"] : 0);
        players.emplace_back(nm, m);
    }
}

// 把 data 寫回 players.json
void Game::writeData() {
    ofstream fout("players.json");
    fout << data.dump(2);
    fout.close();
}

// 找 JSON array 裡面 name 相同的 index，找不到回傳 -1
int Game::findPlayerIndex(const std::string &name) {
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i]["name"] == name) return i;
    }
    return -1;
}

// 玩家登入後：先進商店，再真正 playStage，玩完之後結算 + 存檔
void Game::loginAndPlay() {
    cout << "請輸入玩家名稱：";
    string name;
    cin >> name;

    // 1. 檢查 JSON 裡有沒有這個玩家
    int idx = findPlayerIndex(name);
    if (idx < 0) {
        // JSON 裡沒有，先塞一筆空白進去
        json newEntry = {
            {"name", name},
            {"maxScore", 0},
            {"money", 0},
            {"inventory", json::array()}
        };
        data.push_back(newEntry);
        idx = (int)data.size() - 1;
    }

    // 2. 用 JSON 裡面的 money 建構 Player
    int initialMoney = data[idx]["money"];
    Player p(name, initialMoney);
    // 把這個玩家目前持有的 inventory 也同步給 Player (假設 Player 有方法拿 inventory)
    if (data[idx].contains("inventory")) {
        for (auto &it : data[idx]["inventory"]) {
            p.addToInventory(it.get<std::string>());
        }
    }

    // 3. 強制先進商店
    shop.visit(p);

    // 4. 重置遊戲內部統計
    p.resetStats();

    // 5. 開始真正的 playStage
    playStage(p);

    // 6. 決算：先顯示一些結算資訊（score, play/discard 統計 ...）
    settlementStage(p);

    // 7. 換算貨幣： money_awarded = final_score / 10
    int finalScore   = p.score();
    int moneyAwarded = finalScore / 10;
    cout << "本局最終分數：" << finalScore << "，兌換遊戲幣：" << moneyAwarded << "\n";
    cout << "按 Enter 繼續...";
    string s; 
    getline(cin, s);
    
    system("clear");
    
    // ===== 新增：如果本局分數 > 700，就發放一包小禮包，並立即開出 3 張卡放到玩家 inventory =====
    if (finalScore > 700) {
        cout << "本局得分超過 700 分，獲得一包小禮包！\n";

        // 1. 定義可隨機抽到的功能卡種類
        vector<string> funcPool = {
            "Score2x",
            "Change3ToSpade",
            "Change3ToHeart",
            "Change3ToDiamond",
            "Change3ToClub",
            "Copy1Card"
        };

        // 2. 連抽 3 張（可重複），直接放到 Player::inventory
        cout << "打開小禮包，獲得以下 3 張功能卡：\n";
        for (int i = 0; i < 3; i++) {
            int r = rand() % funcPool.size();
            string chosen = funcPool[r];
            p.addToInventory(chosen);           // 把卡放到 Player 物件的 vector<string> inventory
            cout << "  第 " << (i + 1) << " 張：" << chosen << "\n";
        }
        // 3. 等待玩家看完
        cout << "按 Enter 繼續...";
        getline(cin, s);
        
        system("clear");
    }
    // ===== 新增結束 =====

    // 8. 更新 JSON 中這筆玩家的 money, maxScore, inventory
    //    a. money += moneyAwarded
    data[idx]["money"] = data[idx]["money"].get<int>() + moneyAwarded;
    //    b. inventory 同步回 JSON
    data[idx]["inventory"] = json::array();
    for (auto &it : p.getInventory()) {
        data[idx]["inventory"].push_back(it);
    }
    //    c. 如果本局分數 > maxScore，就更新
    if (finalScore > data[idx]["maxScore"].get<int>()) {
        data[idx]["maxScore"] = finalScore;
    }

    // 9. 把整張 JSON 寫回檔案
    writeData();

    // 10. 同步更新 players vector 中的 money（下一次登入才會看到）
    for (auto &pl : players) {
        if (pl.name() == name) {
            pl.setMoney(data[idx]["money"].get<int>());
            break;
        }
    }
}

// 顯示排行榜，依照 maxScore 排序
void Game::showLeaderBoard() {
    system("clear");
    cout << "=== 排行榜（依 maxScore 由高到低） ===\n";

    // 先把 (maxScore, name) 全蒐集到 vector<pair><>
    vector<pair<int, string>> v;
    for (auto &entry : data) {
        v.push_back({ entry["maxScore"].get<int>(), entry["name"].get<std::string>() });
    }
    // 排序
    sort(v.begin(), v.end(), [](auto &a, auto &b) {
        return a.first > b.first;
    });

    // 印出來
    for (int i = 0; i < (int)v.size(); i++) {
        cout << "Rank " << (i + 1) << "： " << v[i].second
             << "  (Score: " << v[i].first << ")\n";
    }
    cout << "===============================\n";
}

void Game::playStage(Player &p) {
    Deck deck;
    deck.shuffle();

    // 每局開始：重置本局統計 & 抽 8 張手牌
    p.resetStats();
    p.hand().clear();
    for (int i = 0; i < 8; i++) {
        p.hand().add(deck.draw());
    }

    int playRounds    = 0;
    int discardRounds = 0;
    bool usedScore2x  = false; // <— 用來記錄「Score ×2」是否已經使用過（每張票只能用一次）

    while (playRounds < 4 && !p.hand().getCards().empty()) {
        system("clear");
        cout << "============= 遊戲中 =============\n";
        cout << "玩家：" << p.name() << "\n";
        cout << "目前累計分數：" << p.score() << "\n";
        cout << "已出牌次數：" << p.play() << "  已棄牌次數：" << p.discard() << "\n";
        cout << "=================================\n\n";

        // ----- 1. 顯示手牌（ASCII Art，8 張分兩排） -----
        auto &handCards = p.hand().getCards();
        if (handCards.size() < 8) {
            // 保險起見，若少於 8 張就從 deck 補
            while (handCards.size() < 8 && !deck.empty()) {
                p.hand().add(deck.draw());
            }
        }
        // (印出兩排共 8 張的程式碼，可直接 copy 你原本的印法...)
        {
            vector<Card> top4(handCards.begin(), handCards.begin() + 4);
            vector<Card> bot4(handCards.begin() + 4, handCards.begin() + 8);
            vector<vector<string>> topArts, botArts;
            for (int i = 0; i < 4; i++) {
                topArts.push_back(top4[i].getAsciiArt());
                botArts.push_back(bot4[i].getAsciiArt());
            }
            int W = (int)topArts[0][0].length();

            // 第一排
            for (int row = 0; row < 6; row++) {
                for (int c = 0; c < 4; c++) {
                    cout << topArts[c][row] << "  ";
                }
                cout << "\n";
            }
            // 索引 0~3
            for (int c = 0; c < 4; c++) {
                string idxStr = "(" + to_string(c) + ")";
                int padL = (W - (int)idxStr.length()) / 2;
                int padR = W - (int)idxStr.length() - padL;
                cout << string(padL - 5, ' ') << idxStr << string(padR - 5, ' ') << "  ";
            }
            cout << "\n\n";

            // 第二排
            for (int row = 0; row < 6; row++) {
                for (int c = 0; c < 4; c++) {
                    cout << botArts[c][row] << "  ";
                }
                cout << "\n";
            }
            // 索引 4~7
            for (int c = 0; c < 4; c++) {
                int idx = 4 + c;
                string idxStr = "(" + to_string(idx) + ")";
                int padL = (W - (int)idxStr.length()) / 2;
                int padR = W - (int)idxStr.length() - padL;
                cout << string(padL - 5, ' ') << idxStr << string(padR - 5, ' ') << "  ";
            }
            cout << "\n\n";
        }

        // ----- 2. 顯示玩家當前庫存 (inventory) -----
        auto inv = p.getInventory();
        map<string,int> cnt;
        cout << "--- 庫存 (Inventory) ---\n";
        if (inv.empty()) {
            cout << "  (空)\n";
        } else {
            // 統計「不含換行」的項目
            for (auto &it : inv) {
                if (it.find('\n') != string::npos) {
                    // 這筆字串含有 '\n'，視為 ASCII Art，跳過不列出
                    continue;
                }
                cnt[it]++;
            }
            if (cnt.empty()) {
                // 代表目前所有 inventory 都是 ASCII Art，沒有功能卡
                cout << "  (僅含隱藏的文字拼圖，無功能卡可顯示)\n";
            } else {
                int idx = 1;
                for (auto &entry : cnt) {
                    cout << "  [" << idx++ << "] " 
                         << entry.first << " × " << entry.second << "\n";
                }
            }
        }
        cout << "-------------------------\n\n";

        // ----- 3. 本回合可選動作 -----  
        cout << "(1) 出牌   (2) 棄牌   (3) 排序   (4) 結束回合   (5) 提示   (6) 使用道具\n";
        cout << "請輸入： ";
        int cmd;
        cin >> cmd;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // ---------------------------------------------
        // (6) 使用道具：玩家可以從庫存中挑票券使用
        // ---------------------------------------------
        if (cmd == 6) {
            if (inv.empty()) {
                cout << "庫存為空，無法使用道具。\n";
                cout << "按 Enter 返回... ";
                cin.get();
                continue;
            }
            // 印出「庫存票券列表」並讓玩家輸入要用哪種票
            map<int,string> idx2ticket;
            {
                int idx = 1;
                for (auto &entry : cnt) {
                    idx2ticket[idx++] = entry.first;
                }
            }
            cout << "請輸入要使用的票券編號：";
            int chooseIdx;
            cin >> chooseIdx;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (idx2ticket.find(chooseIdx) == idx2ticket.end()) {
                cout << "選擇錯誤。\n";
                cout << "按 Enter 返回... ";
                cin.get();
                continue;
            }

            string ticket = idx2ticket[chooseIdx];
            // 將它從庫存中移除
            p.removeFromInventory(ticket);

            // 同步到 JSON 裡：(在 loginAndPlay() 結算階段會一併寫回文件)
            //   data[idx]["inventory"] = p.getInventory();
            // 但此刻我們先立刻生效「道具效果」

            // ----------------------------
            // 根據票券名稱，執行對應效果
            // ----------------------------
            if (ticket == "Score2x") {
                // 下一次真正出牌得分 * 2
                usedScore2x = true;
                cout << "已啟動 Score×2 票！\n";
            }
            else if (ticket == "Change3ToSpade"
            || ticket == "Change3ToHeart"
            || ticket == "Change3ToDiamond"
            || ticket == "Change3ToClub") 
            {
                // 手牌
                auto &cards = p.hand().getCards();
                if (cards.size() < 3) {
                    cout << "手牌不足 3 張，無法改花色。\n";
                } else {
                    // 1. 根據 ticket 決定 newSuit（使用 enum class Suit）
                    Suit newSuit = Suit::Clubs; // 預設
                    if (ticket == "Change3ToSpade")     newSuit = Suit::Spades;
                    else if (ticket == "Change3ToHeart")   newSuit = Suit::Hearts;
                    else if (ticket == "Change3ToDiamond") newSuit = Suit::Diamonds;
                    else if (ticket == "Change3ToClub")    newSuit = Suit::Clubs;

                    // 2. 隨機選 3 張不同索引
                    int n = (int)cards.size();
                    vector<int> idx(n);
                    iota(idx.begin(), idx.end(), 0);

                    random_device rd;
                    mt19937 gen(rd());
                    shuffle(idx.begin(), idx.end(), gen);

                    // 3. 把前 3 個索引的那三張牌修改花色
                    for (int i = 0; i < 3; i++) {
                        int cardIdx = idx[i];
                        cards[cardIdx].setSuit(newSuit);
                    }

                    // 4. 提示使用結果
                    cout << "已將手牌中隨機 3 張改為 ";
                    switch (newSuit) {
                        case Suit::Spades:   cout << "♠ Spades！\n";    break;
                        case Suit::Hearts:   cout << "♥ Hearts！\n";    break;
                        case Suit::Diamonds: cout << "♦ Diamonds！\n";  break;
                        case Suit::Clubs:    cout << "♣ Clubs！\n";     break;
                    }
                }

                // 5. 從玩家庫存中移除該道具
                p.removeFromInventory(ticket);

                cout << "按 Enter 返回... ";
                cin.get();
                continue;  // 回到本回合選單
            }
            else if (ticket == "Copy1Card") {
                auto &cards = p.hand().getCards();  // cards.size() 必须 == 8
                int n = (int)cards.size();          // 理论上此时 n == 8

                // 如果手牌数不等于 8，就提示并结束（按照需求，手牌永远应该恰好 8 张）
                if (n != 8) {
                    cout << "手牌數異常 (目前 " << n << " 張)，無法使用 Copy Ticket。\n";
                } else {
                    // 1. 随机选一张要复制的牌，srcIdx ∈ [0,7]
                    random_device rd;
                    mt19937 gen(rd());
                    uniform_int_distribution<> disSrc(0, n - 1);
                    int srcIdx = disSrc(gen);

                    // 2. 在剩下的 7 个索引中随机选一个 dstIdx 来覆盖
                    vector<int> choices;
                    choices.reserve(n - 1);
                    for (int i = 0; i < n; i++) {
                        if (i != srcIdx) choices.push_back(i);
                    }
                    shuffle(choices.begin(), choices.end(), gen);
                    int dstIdx = choices[0];

                    // 3. 用拷贝构造把 cards[srcIdx] 复制出来，再覆盖 cards[dstIdx]
                    Card original = cards[srcIdx];
                    Card duplicate(original);  // 拷贝构造，保证 new Card 与原卡各自独立
                    cards[dstIdx] = duplicate;

                    // 4. 提示玩家：复制来源索引和被覆盖索引
                    cout << "已使用 Copy Ticket\n";
                }

                // 5. 从库存中移除 Copy Ticket，并返回到菜单
                //    （确保玩家只能使用一次）
                p.removeFromInventory(ticket);

                cout << "按 Enter 返回... ";
                cin.get();
                continue;
            } 
            else {
                // 不可能，但保險起見
                cout << "未知票券，不做處理。\n";
            }

            cout << "按 Enter 返回... ";
            cin.get(); 
            continue; // 回到本回合的主迴圈，重新顯示手牌與選單
        }

        // ------------------------
        // (1) 出牌
        // ------------------------
        if (cmd == 1 && playRounds < 4) {
            cout << "輸入要出的牌索引（0-based，用空白分開）：";
            string line;
            getline(cin, line);
            stringstream ss(line);
            vector<int> chosenIdx;
            int idxCard;
            while (ss >> idxCard) {
                chosenIdx.push_back(idxCard);
            }

            // 從大到小刪除手牌
            sort(chosenIdx.begin(), chosenIdx.end(), greater<int>());
            vector<Card> playSet;
            for (int id : chosenIdx) {
                playSet.push_back(p.hand().getCards()[id]);
                p.hand().removeAt(id);
            }

            // 計算分數
            int sc = ScoreCalculator::calculate(playSet);
            if (usedScore2x) {
                sc *= 2; 
                usedScore2x = false; 
                cout << "(Score×2 票已使用，本次分數翻倍！)\n";
            }

            string comboName = ScoreCalculator::check(playSet);
            cout << "牌型： " << comboName << "，得分： " << sc << "\n";

            p.recordPlay();
            p.recordScore(sc);
            p.recordPlayType(playSet);

            // 補到 8 張
            for (int k = 0; k < (int)chosenIdx.size(); k++) {
                if (!deck.empty()) 
                    p.hand().add(deck.draw());
            }

            playRounds++;
            cout << "按 Enter 繼續。\n";
            cin.get();
        }
        // ------------------------
        // (2) 棄牌
        // ------------------------
        else if (cmd == 2) {
            if (discardRounds >= 3) {
                cout << "您已經棄牌 3 次，無法再棄牌！按 Enter 繼續。\n";
                cin.get();
                continue;
            }
            cout << "輸入要棄的牌索引（0-based，用空白分開）：";
            string line;
            getline(cin, line);
            stringstream ss(line);
            vector<int> toDiscard;
            int idxCard;
            while (ss >> idxCard) {
                toDiscard.push_back(idxCard);
            }

            sort(toDiscard.begin(), toDiscard.end(), greater<int>());
            for (int id : toDiscard) {
                p.hand().removeAt(id);
            }
            // 棄掉多少張，就抽回多少張
            for (int k = 0; k < (int)toDiscard.size(); k++) {
                if (!deck.empty())
                    p.hand().add(deck.draw());
            }
            p.recordDiscard();
            discardRounds++;
        }
        // ------------------------
        // (3) 排序手牌
        // ------------------------
        else if (cmd == 3) {
            cout << "(1) 依點數排序  (2) 依花色排序：";
            int m; 
            cin >> m;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (m == 1) p.hand().sortByValue();
            else         p.hand().sortBySuit();
        }
        // ------------------------
        // (5) 提示 (Hint)
        // ------------------------
        else if (cmd == 5) {
            auto &hand = p.hand().getCards();
            int bestScore = 0;
            // 用來儲存「經過排序的(牌, 原始index)」組合
            vector<pair<Card,int>> bestComboWithIdx;
            int n = (int)hand.size();

            for (int k = 1; k <= min(5, n); k++) {
                vector<bool> sel(n, false);
                fill(sel.begin(), sel.begin() + k, true);
                do {
                    // 暫存當前選出的牌以及它們在 hand 中的 index
                    vector<pair<Card,int>> candWithIdx;
                    candWithIdx.reserve(k);
                    for (int i = 0; i < n; i++) {
                        if (sel[i]) {
                            candWithIdx.emplace_back(hand[i], i);
                        }
                    }
                    // 將 candWithIdx 按照牌的點數與花色排序，確保 ScoreCalculator 判斷正確
                    sort(candWithIdx.begin(), candWithIdx.end(),
                        [](auto &a, auto &b) {
                            if (a.first.getValue() != b.first.getValue())
                                return a.first.getValue() < b.first.getValue();
                            return a.first.getSuit() < b.first.getSuit();
                        });

                    // 提取純 Card 列表給 ScoreCalculator 使用
                    vector<Card> candCards;
                    candCards.reserve(k);
                    for (auto &pr : candWithIdx) {
                        candCards.push_back(pr.first);
                    }

                    // 計算分數
                    int sc = ScoreCalculator::calculate(candCards);
                    if (sc > bestScore) {
                        bestScore = sc;
                        bestComboWithIdx = candWithIdx; 
                    }
                } while (prev_permutation(sel.begin(), sel.end()));
            }

            cout << "=== 本局提示 ===\n";
            if (bestComboWithIdx.empty()) {
                cout << "無法組成有分數的組合！\n";
            } else {
                cout << "推薦出牌： ";
                // 印出每張卡與它在原手牌中的 index
                for (auto &pr : bestComboWithIdx) {
                    const Card &c = pr.first;
                    int idxInHand = pr.second;
                    cout << c.toString() << "(" << idxInHand << ") ";
                }
                // 再次用純 Card 列表呼叫 check() 取得牌型名稱
                vector<Card> onlyCards;
                onlyCards.reserve(bestComboWithIdx.size());
                for (auto &pr : bestComboWithIdx) {
                    onlyCards.push_back(pr.first);
                }
                cout << "\n對應牌型： " 
                    << ScoreCalculator::check(onlyCards)
                    << "，預估得分： " << bestScore << "\n";
            }
            cout << "按 Enter 繼續。\n";
            cin.get();
        }

        // ------------------------
        // (4) 結束回合／其他
        // ------------------------
        else {
            break; // 跳出本局迴圈，進入結算階段
        }
    }

    // 結算本局統計
    settlementStage(p);
    cout << "按 Enter 繼續..."; 
    string dummy; 
    getline(cin, dummy);
}

void Game::settlementStage(Player &p) {
    system("clear");
    p.printStats();
}