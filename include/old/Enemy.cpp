#include "Enemy.hpp"
#include <iostream>
#include <numeric>
#include <stdexcept>

Enemy::Enemy(std::string name, int hp) : Entity(name, hp) {}

void Enemy::pushActionTable(std::vector<EnemyAction> table) {
    if (table.empty()) return;
    actionStack.push(std::move(table));
}

int Enemy::weightedRoll(const std::vector<EnemyAction>& table) const {
    int totalWeight = 0;
    for (const auto& a : table) totalWeight += a.weight;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, totalWeight - 1);
    int roll = dist(rng);

    int cumulative = 0;
    for (size_t i = 0; i < table.size(); ++i) {
        cumulative += table[i].weight;
        if (roll < cumulative) return static_cast<int>(i);
    }
    return static_cast<int>(table.size() - 1);
}

int Enemy::rollDamage(int min, int max) const {
    if (min == max) return min;
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void Enemy::rollIntent() {
    if (actionStack.empty()) {
        currentIntent = {"(idle)", 0, 0, "", 0, 1, {}, false};
        return;
    }
    const auto& table = actionStack.top();
    int idx = weightedRoll(table);
    currentIntent = table[idx];
}

const EnemyAction& Enemy::getIntent() const {
    return currentIntent;
}

void Enemy::executeIntent(Entity& target) {
    std::cout << "\n[ENEMY] " << getName() << " uses " << currentIntent.name <<
"!" << std::endl;

    if (currentIntent.maxDamage > 0) {
        int dmg = rollDamage(currentIntent.minDamage, currentIntent.maxDamage);
        target.takeDamage(dmg);
    }

    if (!currentIntent.status.empty()) {
        target.addStatus(currentIntent.status, currentIntent.statusDuration);
    }

    if (!currentIntent.pushTable.empty()) {
        std::cout << " -> " << getName() << " is preparing something..." <<
std::endl;
        pushActionTable(currentIntent.pushTable);
    }

    if (currentIntent.popAfter && !actionStack.empty()) {
        actionStack.pop();
    }
}