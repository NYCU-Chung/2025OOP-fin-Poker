#include "Shop.h"
#include <bits/stdc++.h>
#include <cstdlib>   // for rand(), srand()
#include <ctime>     // for time()
using namespace std;

Shop::Shop(json &d)
    : data(d)
{}

void Shop::visit(Player &p) {
    int idx = findPlayerIndex(p.name());
    if (idx < 0) return;  // 如果找不到，直接離開

    // 確保 JSON 物件有 money / inventory 欄位
    auto &entry = data[idx];
    if (!entry.contains("money"))      entry["money"] = 0;
    if (!entry.contains("inventory"))  entry["inventory"] = json::array();

    while (true) {
        system("clear");
        cout << "=== 商店 ===\n";
        cout << "玩家： " << p.name() << "  目前餘額： " << entry["money"] << "  \n\n";
        showItems();
        cout << "0. 離開商店\n";
        cout << "請選擇要買的商品編號：";
        int choice;
        cin >> choice;
        if (choice == 0) break;
        buyItem(p, choice);

        cout << "按 Enter 繼續...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}

// 列出所有可買的東西
void Shop::showItems() {
    cout << "1. score x2    (價格 10)\n";
    cout << "2. change 3 cards to ♠    (價格 5)\n";
    cout << "3. change 3 cards to \033[31m♥\033[0m    (價格 5)\n";
    cout << "4. change 3 cards to \033[31m♦\033[0m    (價格 5)\n";
    cout << "5. change 3 cards to ♣    (價格 5)\n";
    cout << "6. copy 1 card     (價格 8)\n";
    cout << "7. 小禮包 (隨機 3 張功能卡)    (價格 5)\n";
    cout << "8. 大禮包 (隨機 1 張蒐集卡)      (價格 10)\n";
}

// 依照使用者輸入的 choice 決定扣錢 + 入庫存
void Shop::buyItem(Player &p, int choice) {
    int idx = findPlayerIndex(p.name());
    if (idx < 0) return;
    auto &entry = data[idx];
    int  money = entry["money"].get<int>();
    string item;
    int   price = 0;

    switch (choice) {
        case 1: item = "Score2x";       price = 10; break;
        case 2: item = "Change3ToSpade"; price = 5;  break;
        case 3: item = "Change3ToHeart"; price = 5;  break;
        case 4: item = "Change3ToDiamond"; price = 5; break;
        case 5: item = "Change3ToClub";  price = 5;  break;
        case 6: item = "Copy1Card";     price = 8;  break;
        case 7: price = 5;  break;
        case 8: price = 10;  break;
        default:
            cout << "無效的商品編號！\n";
            return;
    }

    if (money < price) {
        cout << "餘額不足，無法購買。\n";
        return;
    }

    // 扣錢
    entry["money"] = money - price;
    
    // 如果是 1~6 的功能卡，就像原本一樣把 item 字串放進 inventory
    if (choice >= 1 && choice <= 6) {
        // 推進 JSON
        entry["inventory"].push_back(item);
        // 同步到 Player 物件
        p.addToInventory(item);

        cout << "已購買：" << item << "。\n";
    }
    // 如果是 7：「小禮包」，改為隨機抽出 3 張功能卡
    else if (choice == 7) {
        // 定義所有可隨機出的功能卡池
        vector<string> funcPool = {
            "Score2x",
            "Change3ToSpade",
            "Change3ToHeart",
            "Change3ToDiamond",
            "Change3ToClub",
            "Copy1Card"
        };

        cout << "打開「小禮包」！您獲得了：\n";
        for (int i = 0; i < 3; i++) {
            // 在 funcPool 裡隨機挑一張（可重複）
            int r = rand() % funcPool.size();
            string chosen = funcPool[r];

            // 推進 JSON
            entry["inventory"].push_back(chosen);
            // 同步到 Player 物件
            p.addToInventory(chosen);

            // 顯示抽到的結果
            cout << "  第 " << (i+1) << " 張： " << chosen << "\n";
        }
    }
    // ===== 處理第 8 項：大禮包 (隨機 1 張蒐集卡) =====
    else if (choice == 8) {
        // (1) 定義 4 種蒐集卡 (ASCII Art)，用 raw string literal 保留換行
        const string artSpade = R"(
   ♠
  ♠♠♠
 ♠♠♠♠♠
  ♠♠♠
   ♠
)";
        const string artHeart = R"(
  ♥   ♥
 ♥ ♥ ♥ ♥
 ♥  ♥  ♥
  ♥   ♥
   ♥ ♥
    ♥
)";
        const string artDiamond = R"(
    ♦
   ♦ ♦
  ♦   ♦
   ♦ ♦
    ♦
)";
        const string artClub = R"(
   ♣
  ♣ ♣
 ♣ ♣ ♣
   ♣
   ♣
)";
        // (2) 把這四種放在同一個池子裡
        vector<pair<string, const string*>> bigArtPool = {
            {"Spade",   &artSpade},
            {"Heart",   &artHeart},
            {"Diamond", &artDiamond},
            {"Club",    &artClub}
        };

        // 隨機抽 0~3
        int r = rand() % bigArtPool.size();
        const string &chosenArt = *bigArtPool[r].second;

        // 顯示給玩家看
        cout << "打開「大禮包」！您獲得了一張蒐集卡：\n";
        cout << chosenArt << "\n";

        // (3) 存到 JSON 的 inventory 裡，以及 Player 物件的 inventory
        entry["inventory"].push_back(chosenArt);
        p.addToInventory(chosenArt);

        bool collectedBefore = entry.value("bigPackCollected", false);
        // (4) 檢查是否已經湊齊四種不同的蒐集卡
        //     如果還沒領過獎 (bigPackCollected == false)，而 inventory 裡包含 Spade/Heart/Diamond/Club
        //     就加 500 元，並把 bigPackCollected 設為 true
        bool hasSpade = false, hasHeart = false, hasDiamond = false, hasClub = false;
        for (auto &it : entry["inventory"]) {
            if (it.get<string>() == artSpade)   hasSpade = true;
            if (it.get<string>() == artHeart)   hasHeart = true;
            if (it.get<string>() == artDiamond) hasDiamond = true;
            if (it.get<string>() == artClub)    hasClub = true;
        }

        if (hasSpade && hasHeart && hasDiamond && hasClub && !collectedBefore) {
            // 領獎 500 元
            entry["money"] = entry["money"].get<int>() + 500;
            entry["bigPackCollected"] = true;
            cout << "恭喜您已經集齊四張蒐集卡！獲得 500 元獎勵！\n";
        }
    }
/*
    // 加到 JSON 裡的 inventory
    entry["inventory"].push_back(item);
    // 同步到 Player 物件
    p.addToInventory(item);


    cout << "已購買：" << item << " 。\n";
*/
}

// 找 JSON 裡面 name 相同的 index
int Shop::findPlayerIndex(const string &name) {
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i]["name"] == name) return i;
    }
    return -1;
}
