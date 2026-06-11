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

	virtual void OnPlayerHealthChange(int newHP, int newMaxHP) = 0;
	virtual void OnEnemyHealthChange(int newHP, int newMaxHP) = 0;

	virtual void OnPlayerDeath() = 0;
	virtual void OnEnemyDeath() = 0;

	virtual void OnCardUpdate(ObjectID cardId, CardValue value, Suit suit, std::string name, std::string description) = 0;

	virtual void OnCardPlacedOnTable(ObjectID cardID) = 0;
	virtual void OnCardDrawToTable(std::vector<ObjectID> cards) = 0;

	virtual void OnCardsCaptured(std::vector<ObjectID> cards) = 0;

	virtual void OnPlayerBeginTurn(int turnCount) = 0;
	virtual void OnPlayerEndTurn()   = 0;

	virtual void OnEnemyBeginTurn() = 0;
	virtual void OnEnemyEndTurn()   = 0;
};
