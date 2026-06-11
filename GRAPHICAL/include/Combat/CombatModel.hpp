#pragma once

#include <map>
#include <memory>
#include <string>
#include <set>

#include <Combat/CharacterState.hpp>
#include <Combat/CombatHelper.hpp>
#include <Combat/CombatModelHelper.hpp>
#include <Combat/CombatModelListener.hpp>

#include <Combat/Entity.hpp>
#include <Combat/Enemy.hpp>

#include <Card.hpp>

class CombatModel
{
public:
	void Init();

	void BeginPlayerTurn();
	void BeginEnemyTurn();

	void ShuffleDeck();

	void Draw(size_t count);
	void DrawToTable(size_t count);
	void DrawUntilHandFull();
	void Discard(ObjectID cardID);
	void Exhaust(ObjectID cardID);
	void DiscardHand();

	void StartPlayerTurn();
	void EndPlayerTurn();

	void PlayCard(ObjectID cardID);

	void Damage(Entity * dst, uint32_t calculatedDamage, Entity * source, bool isEcho);
	void Death(ObjectID source, ObjectID target);

	void UpdateEntitiesState();

	size_t EffectiveHandSize() { return m_handSize; }

	std::string GetCardDescription(ObjectID card);
	std::string GetCardName(ObjectID card);

	void RegisterCard(ObjectID cardID, Card * card);
	CombatModel(CombatModelListener * listener);

	void ChangeCardSuit(ObjectID card, Suit suit);
	void ChangeCardSuit(Card * card, Suit suit);

	std::vector<int> FindCaptureIndices(
		const Card * playedCard,
		int customTargetVal = -1
	);

	ObjectID GetObjectID(Card * card)
	{
		if(m_cardCO.find(card) == m_cardCO.end())
			return InvalidObject;

		return m_cardCO.at(card);
	}

	Card * GetCard(ObjectID card)
	{
		if(m_cardOC.find(card) == m_cardOC.end())
			return nullptr;

		return m_cardOC.at(card);
	}

private:
	void PrintPiles();
	std::vector<ObjectID> Convert(std::vector<Card *> cards)
	{
		std::vector<ObjectID> _converted;
		
		for(auto card : cards)
		_converted.emplace_back(m_cardCO.at(card));
		
		return _converted;
	}

	std::vector<Card *> Convert(std::vector<ObjectID> cards)
	{
		std::vector<Card *> _converted;
		
		for(auto card : cards)
		_converted.emplace_back(m_cardOC.at(card));
		
		return _converted;
	}

	std::map<Card *, ObjectID> m_cardCO;
	std::map<ObjectID, Card *> m_cardOC;

	std::vector<Card *> m_drawPile; // all cards should originate from here

	std::vector<Card *> m_hand;
	std::vector<Card *> m_table;

	std::vector<Card *> m_discardPile;
	std::vector<Card *> m_exhaustPile;
	std::vector<Card *> m_capturePile;

	size_t m_handSize = 3;
	size_t m_maxHandSize = 10;

	CombatModelListener * m_listener;

	std::unique_ptr<Entity> m_player;
	std::unique_ptr<Enemy>   m_enemy;

	int scopaCount = 0;
    int turn = 0;
	int playerCaptures = 0;
	int enemyCaptures  = 0;
};
