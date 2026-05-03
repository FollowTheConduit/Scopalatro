#pragma once

#include <string>
#include <vector>
#include <stack>
#include <random>
#include "Entity.hpp"

struct EnemyAction {
    std::string name;
    int minDamage = 0;
    int maxDamage = 0;
    std::string status = "";
    int statusDuration = 0;
    int weight = 1;

    std::vector<EnemyAction> pushTable; // if not empty, push as the new action table
    bool popAfter = false; // if true, pop the current table
};

class Enemy : public Entity {
public:
    Enemy(std::string name, int hp);

    void pushActionTable(std::vector<EnemyAction> table);
    void rollIntent();
    const EnemyAction& getIntent() const;
    void executeIntent(Entity& target);

private:
    std::stack<std::vector<EnemyAction>> actionStack;
    EnemyAction currentIntent;

    int rollDamage(int min, int max) const;
    int weightedRoll(const std::vector<EnemyAction>& table) const;
};