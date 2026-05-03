#include <chrono>
#include <random>

#include <Combat/CombatModel.hpp>

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

	m_drawPile = cards;
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

CombatModel::CombatModel (CombatModelListener * listener, IndexedCardInventory cardTable):
	m_listener {listener},
	m_cardTable {cardTable}
{
	for (auto & [ID, card] : m_cardTable)
	{
		m_drawPile.emplace_back (ID);
	}
}
