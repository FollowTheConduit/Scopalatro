#include <algorithm>
#include <chrono>
#include <random>

#include <Combat/CombatModel.hpp>

#include <Core/Logger.hpp>

using namespace TLOT;

void CombatModel::PrintPiles ()
{
	std::string line = "(";

	for (auto & cardID : m_drawPile)
	{
		auto & card = m_cardTable[cardID];
		line += card->GetName () + ", ";
	}
	line += ")\n(";
	for (auto & cardID : m_hand)
	{
		auto & card = m_cardTable[cardID];
		line += card->GetName () + ", ";
	}
	line += ")\n(";
	for (auto & cardID : m_discardPile)
	{
		auto & card = m_cardTable[cardID];
		line += card->GetName () + ", ";
	}
	line += ")";

	Logger::log (LogLevel::Info, "[{}]", line);
}

void CombatModel::ShuffleDeck ()
{
	std::vector<ObjectID> cards;

	
	if (!m_discardPile.empty ())
	{
		m_listener->OnDiscardToDrawPile (m_discardPile);
	}
	
	cards.insert (cards.end (), m_drawPile.begin    (), m_drawPile.end    ());
	cards.insert (cards.end (), m_discardPile.begin (), m_discardPile.end ());
	
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

	m_drawPile.clear ();
	m_discardPile.clear ();

	m_drawPile = cards;

	PrintPiles ();
}

void CombatModel::Draw (size_t count)
{
	std::vector<ObjectID> drawnCards;

	while (drawnCards.size () < std::min (count, m_maxHandSize))
	{
		if (m_drawPile.empty ())
		{
			ShuffleDeck ();

			if (m_drawPile.empty ())
			{
				m_listener->OnMessage ("draw_pile_empty");
				break;
			}
		}

		auto card = m_drawPile.back ();
		m_drawPile.pop_back ();

		m_hand.push_back (card);

		drawnCards.push_back (card);
	}

	m_listener->OnCardsDrawn (drawnCards);

	PrintPiles ();
}

void CombatModel::DrawUntilHandFull ()
{
	std::vector<ObjectID> drawnCards;

	while (m_hand.size () <= m_maxHandSize)
	{
		if (m_drawPile.empty ())
		{
			ShuffleDeck ();

			if (m_drawPile.empty ())
			{
				m_listener->OnMessage ("draw_pile_empty");
				break;
			}
		}

		auto card = m_drawPile.back ();
		m_drawPile.pop_back ();

		m_hand.push_back (card);

		drawnCards.push_back (card);
	}

	m_listener->OnCardsDrawn (drawnCards);

	PrintPiles ();
}

void CombatModel::Discard (ObjectID cardID)
{
	Logger::log (LogLevel::Info, "trying to discard: {}", cardID);

	std::vector<ObjectID> hand;
	std::vector<ObjectID> discarded;

	for (auto _cardID : m_hand)
	{
		if (cardID != _cardID) hand.push_back (_cardID);
		else discarded.push_back (_cardID);
	}

	if (discarded.empty ())
	{
		Logger::log (LogLevel::Error, "why empty");
	}

	m_hand = hand;
	m_discardPile.insert (m_discardPile.end (), discarded.begin (), discarded.end ());

	m_listener->OnCardsDiscarded (discarded);

	PrintPiles ();
}

void CombatModel::Exhaust (ObjectID cardID)
{

}

void CombatModel::DiscardHand ()
{
	std::vector<ObjectID> discardedCards;

	while (m_hand.size () != 0)
	{
		auto card = m_hand.back ();
		m_hand.pop_back ();

		m_discardPile.emplace_back (card);
	}

	m_listener->OnCardsDiscarded (discardedCards);

	PrintPiles ();
}

void CombatModel::StartPlayerTurn ()
{
	// Resolve effects and statuses

	ShuffleDeck ();
	Draw (m_handSize);
}

void CombatModel::EndPlayerTurn ()
{
	// Resolve effects and statuses

	DiscardHand ();
}

void CombatModel::PlayCard (ObjectID cardID)
{
	// logic here

	bool exhaust = false;
	if (exhaust)
	{
		Exhaust (cardID);
	}
	else
	{
		Discard (cardID);
	}
}

void CombatModel::Damage (ObjectID source, uint32_t calculatedDamage, ObjectID target)
{
	// Resolve effects and statuses

	m_listener->OnDamage ();
}

void CombatModel::Death (ObjectID source, ObjectID target)
{
	// Resolve effects and statuses
}

std::string_view CombatModel::GetCardDescription (ObjectID card)
{
	return std::string_view {"If you see this, I forgor to implement GetCardDescription (ObjectID Card)"};
}

void CombatModel::RegisterCard(ObjectID cardID, Card * card)
{
	m_cardTable.emplace(cardID, card);

	m_drawPile.emplace_back(cardID);
}

CombatModel::CombatModel (CombatModelListener * listener):
	m_listener {listener}
{
}

