#ifndef SHOP_H
#define SHOP_H

#include "Player.h"
#include "include/json.hpp"
#include <string>
#include <vector>

using json = nlohmann::json;

class Shop {
public:
    Shop(json &d);
    void visit(Player &p);

private:
    json &data;   // 直接引用 Game 裡的 data

    void showItems();
    void buyItem(Player &p, int choice);
    int  findPlayerIndex(const std::string &name);
};

#endif // SHOP_H
