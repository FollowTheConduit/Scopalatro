#include <algorithm>
#include <chrono>
#include <random>

#include <Combat/CombatModel.hpp>

#include <Core/Logger.hpp>

using namespace TLOT;

void CombatModel::PrintPiles()
{
	std::string line = "(";

	for(auto & card : m_drawPile)
	{
		line += card->GetName() + ", ";
	}
	line += ")\n(";
	for(auto & card : m_hand)
	{
		line += card->GetName() + ", ";
	}
	line += ")\n(";
	for(auto & card : m_discardPile)
	{
		line += card->GetName() + ", ";
	}
	line += ")";

	Logger::log(LogLevel::Info, "[{}]", line);
}

void CombatModel::ShuffleDeck()
{
	std::vector<Card *> cards;

	if(!m_discardPile.empty())
	{
		m_listener->OnDiscardToDrawPile(Convert(m_discardPile));
	}
	
	cards.insert(cards.end(), m_drawPile.begin   (), m_drawPile.end   ());
	cards.insert(cards.end(), m_discardPile.begin(), m_discardPile.end());
	
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

	m_drawPile.clear();
	m_discardPile.clear();

	m_drawPile = cards;

	PrintPiles();
}

void CombatModel::Draw(size_t count)
{
	std::vector<Card *> drawnCards;

	while(drawnCards.size() < std::min(count, m_maxHandSize))
	{
		if(m_drawPile.empty())
		{
			ShuffleDeck();

			if(m_drawPile.empty())
			{
				m_listener->OnMessage("draw_pile_empty");
				break;
			}
		}

		auto card = m_drawPile.back();
		m_drawPile.pop_back();

		m_hand.push_back(card);

		drawnCards.push_back(card);
	}

	m_listener->OnCardsDrawn(Convert(drawnCards));

	PrintPiles();
}

void CombatModel::DrawUntilHandFull()
{
	std::vector<Card *> drawnCards;

	while(m_hand.size() <= m_maxHandSize)
	{
		if(m_drawPile.empty())
		{
			ShuffleDeck();

			if(m_drawPile.empty())
			{
				m_listener->OnMessage("draw_pile_empty");
				break;
			}
		}

		auto card = m_drawPile.back();
		m_drawPile.pop_back();

		m_hand.push_back(card);

		drawnCards.push_back(card);
	}

	m_listener->OnCardsDrawn(Convert(drawnCards));

	PrintPiles();
}

void CombatModel::Discard(ObjectID cardID)
{
	Logger::log(LogLevel::Info, "trying to discard: {}", cardID);

	std::vector<Card *> hand;
	std::vector<Card *> discarded;

	auto card = m_cardOC.at(cardID);

	for(auto _card : m_hand)
	{
		if(_card != card) hand.push_back(_card);
		else discarded.push_back(_card);
	}

	if(discarded.empty())
	{
		Logger::log(LogLevel::Error, "why empty");
	}

	m_hand = hand;
	m_discardPile.insert(m_discardPile.end(), discarded.begin(), discarded.end());

	m_listener->OnCardsDiscarded(Convert(discarded));

	PrintPiles();
}

void CombatModel::Exhaust(ObjectID cardID)
{

}

void CombatModel::DiscardHand()
{
	std::vector<ObjectID> discardedCards;

	while(m_hand.size() != 0)
	{
		auto card = m_hand.back();
		m_hand.pop_back();

		m_discardPile.emplace_back(card);
	}

	m_listener->OnCardsDiscarded(discardedCards);

	PrintPiles();
}

void CombatModel::StartPlayerTurn()
{
	// Resolve effects and statuses

	ShuffleDeck();
	Draw(m_handSize);
}

void CombatModel::EndPlayerTurn()
{
	// Resolve effects and statuses

	DiscardHand();
}

void CombatModel::PlayCard(ObjectID cardID)
{
	// logic here
	auto card = m_cardOC.at(cardID);
	if (!card)
		return;


	std::vector<Entity*> targets = { m_enemy.get() };

	EffectContext ctx;
	ctx.source       = m_player.get();
	ctx.targets      = targets;
	ctx.cardSource   = card;
	ctx.scorePile    = &m_capturePile;
	ctx.table        = &m_table;
	ctx.observer     = this;

	for (auto & effect : card->GetEffects())
	{
		std::cout << " [Effect]: " << effect->getDescription() << std::endl;
		effect->apply(ctx);
	}

	int targetVal = card->GetNumericValue();

	if (card->HasEffects())
	{
		targetVal += m_player->getSpellValueBoost();
	}

	auto capturedIndices = FindCaptureIndices(card, targetVal);

	std::vector<Card *> captured;
	captured.emplace_back(card);

	if (!capturedIndices.empty())
	{
		playerCaptures++;
        std::cout << " -> CAPTURE! ";

		CaptureEvent evt;
		evt.playedCard = card;
		evt.player     = m_player.get();
		evt.enemy      = m_enemy .get();
		evt.scorePile  = &m_capturePile;

        for (int idx : capturedIndices)
		{
            std::cout << m_table[idx]->GetName() << " ";

			evt.captured.push_back(m_table[idx]);
            m_capturePile.push_back(std::move(m_table[idx]));
        }

        std::cout << "!" << std::endl;

		//for (auto& r : m_player->getRelics()) {
		//	r->onCapture(evt);
		//}

        std::sort(capturedIndices.rbegin(), capturedIndices.rend());

        for (int idx : capturedIndices)
		{
			captured.emplace_back(m_table.at(idx));
            m_table.erase(m_table.begin() + idx);
		}
        
        if (m_table.empty())
		{
            ++scopaCount;
            std::cout
			<< " SCOPA! Table cleared ("
			<< scopaCount
			<< " sweep"
			<< (scopaCount > 1 ? "s" : "")
			<< " total) "
			<< std::endl;
        }

		captured.emplace_back(card);
		m_listener->OnCardsCaptured(Convert(captured));
    }
	else
	{
        std::cout << " -> No capture. Card added to table." << std::endl;
        m_table.push_back(card);

		auto it = std::find(m_hand.begin(), m_hand.end(), card);
		m_hand.erase(it);

		m_listener->OnCardPlacedOnTable(GetObjectID(card));
    }


	//bool exhaust = false;
	//if(exhaust)
	//{
	//	Exhaust(cardID);
	//}
	//else
	//{
	//	Discard(cardID);
	//}
}

void CombatModel::Damage(Entity * dst, uint32_t calculatedDamage, Entity * source, bool isEcho)
{
	// Resolve effects and statuses
	dst->takeDamage(calculatedDamage, source, isEcho);

	if(dst == m_player.get())
	{
		m_listener->OnPlayerDamage(dst->getHp(), dst->getMaxHp());
	}
	else
	{
		m_listener->OnEnemyDamage(0);
	}

}

void CombatModel::Death(ObjectID source, ObjectID target)
{
	// Resolve effects and statuses
}

void CombatModel::UpdateEntitiesState()
{
	if(!m_player->isAlive())
	{
		m_listener->OnPlayerDeath();
	}
	else if(!m_enemy->isAlive())
	{
		m_listener->OnEnemyDeath();
	}
}

std::string CombatModel::GetCardDescription(ObjectID card)
{
	if(m_cardOC.find(card) == m_cardOC.end())
		return "invalid cardID";

	return m_cardOC.at(card)->GetDescription();
}

void CombatModel::RegisterCard(ObjectID cardID, Card * card)
{
	m_cardOC.emplace(cardID, card);
	m_cardCO.emplace(card, cardID);

	m_drawPile.emplace_back(card);
}

std::vector<int> CombatModel::FindCaptureIndices(
	const Card* playedCard,
	int customTargetVal
) {
	
	int targetVal = customTargetVal > 0 ? customTargetVal : playedCard->GetNumericValue();

    for(size_t i = 0; i < m_table.size(); ++i) {
        if(m_table[i]->GetNumericValue() == targetVal) {
            return { static_cast<int>(i) };
        }
    }

    int n = m_table.size();
    for(int i = 1; i <(1 << n); ++i) {
        int currentSum = 0;
        std::vector<int> indices;
        for(int j = 0; j < n; ++j) {
            if((i >> j) & 1) {
                currentSum += m_table[j]->GetNumericValue();
                indices.push_back(j);
            }
        }
        if(currentSum == targetVal) {
            return indices;
        }
    }        

    return {};
}

void CombatModel::ChangeCardSuit(ObjectID cardID, Suit suit)
{
	auto card = GetCard(cardID);
	if (card)
		ChangeCardSuit(card, suit);

}

void CombatModel::ChangeCardSuit(Card * card, Suit suit)
{
	card->SetSuit(suit);
	m_listener->OnCardUpdate(GetObjectID(card), card->GetValue(), card->GetSuit());
}


CombatModel::CombatModel(CombatModelListener * listener):
	m_listener {listener}
{

}

