#pragma once

#include <map>
#include <memory>
#include <string>
#include <set>

#include <Combat/CharacterState.hpp>
#include <Combat/CombatHelper.hpp>
#include <Combat/CombatModelHelper.hpp>
#include <Combat/CombatModelListener.hpp>

#include <Card.hpp>

class CombatModel
{
public:
	void ShuffleDeck ();

	void Draw (size_t count);
	void DrawUntilHandFull ();
	void Discard (ObjectID cardID);
	void Exhaust (ObjectID cardID);
	void DiscardHand ();

	void StartPlayerTurn ();
	void EndPlayerTurn ();

	void PlayCard (ObjectID cardID);

	void Damage (ObjectID source, uint32_t calculatedDamage, ObjectID target);
	void Death  (ObjectID source, ObjectID target);

	size_t EffectiveHandSize () { return m_handSize; }

	std::string_view GetCardDescription (ObjectID card);
	CombatModel (CombatModelListener * listener, IndexedCardTable cardTable);

private:
	void PrintPiles ();

	CharacterState player;

	IndexedCardTable m_cardTable;

	std::vector<ObjectID> m_drawPile; // all cards should originate from here

	std::vector<ObjectID> m_hand;
	std::vector<ObjectID> m_discardPile;
	std::vector<ObjectID> m_exhaustPile;
	std::vector<ObjectID> m_capturePile;

	size_t m_handSize = 3;
	size_t m_maxHandSize = 10;

	CombatModelListener * m_listener;
};
