#pragma once

#include <set>

#include <Combat/Card.hpp>
#include <Combat/CombatHelper.hpp>
#include <Combat/CombatModelHelper.hpp>
#include <Combat/CombatViewHelper.hpp>

struct EnnemyParam
{
	
};

struct CombatParams
{
	uint32_t playerHP;
	uint32_t playerMaxHP;

	CardInventory playerCards;
	ItemInventory playerItems;

	std::vector<EnnemyParam> enemies;
};
