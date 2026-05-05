#pragma once

#include <string>
#include <vector>
#include <stack>
#include <random>

struct EnemyAction;
using EnemyActionTable = std::vector<EnemyAction>;

struct EnemyAction
{
	EnemyAction () = default;
	EnemyAction (int minDamage, int maxDamage, int weight);
	EnemyAction (int minDamage, int maxDamage, int weight, EnemyActionTable table);

	int minDamage = 0;
    int maxDamage = 0;
	
    int weight = 1;
	
    std::string name;
    EnemyActionTable pushTable; // if not empty, push as the new action table
    bool popAfter = false; // if true, pop the current table
};

class Enemy {
public:
    Enemy (std::string name, int hp);

    void PushActionTable (EnemyActionTable table);
    void RollIntent ();

    EnemyAction const & GetIntent() const;

	std::string name;

private:
    std::stack<EnemyActionTable> m_actionStack;
    EnemyAction m_currentIntent;

    int RollDamage (int min, int max) const;
    int WeightedRoll (EnemyActionTable const & table) const;
};