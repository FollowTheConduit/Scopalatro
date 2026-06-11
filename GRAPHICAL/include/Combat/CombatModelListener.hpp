#pragma once

#include <string>
#include <vector>

#include <Combat/CombatHelper.hpp>

struct DamageContext;
struct DeathContext;

class CombatModelListener
{
public:
	virtual void OnMessage(std::string const message) = 0;

	virtual void OnDiscardToDrawPile(std::vector<ObjectID> cards) = 0;

	virtual void OnCardsDrawn(std::vector<ObjectID> cards) = 0;
	virtual void OnCardsDiscarded(std::vector<ObjectID> cards) = 0;

	virtual void OnPlayerDamage(int newHP, int newMaxHP) = 0;
	virtual void OnEnemyDamage(int amount) = 0;

	virtual void OnPlayerDeath() = 0;
	virtual void OnEnemyDeath() = 0;

	virtual void OnCardUpdate(ObjectID cardId, CardValue value, Suit suit) = 0;

	virtual void OnCardPlacedOnTable(ObjectID cardID) = 0;

	virtual void OnCardsCaptured(std::vector<ObjectID> cards) = 0;
};
