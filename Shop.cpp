#include "Shop.h"
#include <bits/stdc++.h>
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
    // 加到 JSON 裡的 inventory
    entry["inventory"].push_back(item);
    // 同步到 Player 物件
    p.addToInventory(item);

    cout << "已購買：" << item << " 。\n";
}

// 找 JSON 裡面 name 相同的 index
int Shop::findPlayerIndex(const string &name) {
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i]["name"] == name) return i;
    }
    return -1;
}
