#pragma once

#include <Combat/CombatHelper.hpp>
#include <vector>

struct DamageContext;
struct DeathContext;

class CombatModelListener
{
public:
	virtual void OnMessage (std::string const message) = 0;

	virtual void OnDiscardToDrawPile (std::vector<ObjectID> cards) = 0;

	virtual void OnCardsDrawn     (std::vector<ObjectID> cards) = 0;
	virtual void OnCardsDiscarded (std::vector<ObjectID> cards) = 0;

	virtual void OnDamage () = 0;
	virtual void OnDeath  () = 0;
};
