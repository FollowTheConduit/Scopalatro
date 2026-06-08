#include <Combat/Enemy.hpp>

#include <iostream>
#include <numeric>
#include <stdexcept>

EnemyAction::EnemyAction (int minDamage, int maxDamage, int weight): minDamage {minDamage}, maxDamage {maxDamage}, weight {weight} {}
EnemyAction::EnemyAction (int minDamage, int maxDamage, int weight, EnemyActionTable table): minDamage {minDamage}, maxDamage {maxDamage}, weight {weight}, pushTable {table} {}

Enemy::Enemy(std::string name, int hp): name{name} {}

void Enemy::PushActionTable(EnemyActionTable table)
{
    if (table.empty ()) return;

    m_actionStack.push (std::move (table));
}

int Enemy::WeightedRoll(EnemyActionTable const & table) const
{
    int totalWeight = 0;
    for (const auto& a : table) totalWeight += a.weight;

    std::random_device rd;
    std::mt19937 rng (rd ());
    std::uniform_int_distribution<int> dist (0, totalWeight - 1);
    int roll = dist (rng);

    int cumulative = 0;
    for (size_t i = 0; i < table.size (); ++i)
	{
        cumulative += table[i].weight;
        if (roll < cumulative) return static_cast<int> (i);
    }

    return static_cast<int> (table.size () - 1);
}

int Enemy::RollDamage (int min, int max) const
{
    if (min == max) return min;
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void Enemy::RollIntent()
{
    if (m_actionStack.empty()) {
        m_currentIntent = EnemyAction {};
        return;
    }
    const auto& table = m_actionStack.top();
    int idx = WeightedRoll(table);
    m_currentIntent = table[idx];
}

EnemyAction const & Enemy::GetIntent() const
{
    return m_currentIntent;
}

//void Enemy::executeIntent(Entity& target) {
//    std::cout << "\n[ENEMY] " << getName() << " uses " << currentIntent.name <<
//"!" << std::endl;
//
//    if (currentIntent.maxDamage > 0) {
//        int dmg = rollDamage(currentIntent.minDamage, currentIntent.maxDamage);
//        target.takeDamage(dmg);
//    }
//
//    if (!currentIntent.status.empty()) {
//        target.addStatus(currentIntent.status, currentIntent.statusDuration);
//    }
//
//    if (!currentIntent.pushTable.empty()) {
//        std::cout << " -> " << getName() << " is preparing something..." <<
//std::endl;
//        pushActionTable(currentIntent.pushTable);
//    }
//
//    if (currentIntent.popAfter && !actionStack.empty()) {
//        actionStack.pop();
//    }
//}