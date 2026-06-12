#include <algorithm>
#include <chrono>
#include <random>

#include <Combat/CombatModel.hpp>

#include <Core/Logger.hpp>

using namespace TLOT;

CombatModel::CombatModel(CombatModelListener * listener):
	m_listener {listener}
{

}

void CombatModel::RegisterPlayerCard(Card * card)
{
	m_registeredCards.emplace(card);
	m_playerPiles.insert(card, CardPiles::Draw);
}

void CombatModel::RegisterEnemyCard(Card * card)
{
	m_registeredCards.emplace(card);
	m_enemyPiles.insert(card, CardPiles::Draw);
}


void CombatModel::Init()
{
	m_player = std::make_unique<Entity>("player", 50);
	m_enemy  = std::make_unique<Enemy>("Badalisk", 30, 0.5f);

	m_listener->OnPlayerHealthChange(m_player->getHp(), m_player->getMaxHp());
	m_listener->OnEnemyHealthChange(m_enemy->getHp(), m_enemy->getMaxHp());

	m_enemy->buildDeck(m_enemyPiles.drawPile);

	m_playerPiles.shuffle();
	m_enemyPiles.shuffle();

	DrawToTable(3);
	BeginPlayerTurn();
}

void CombatModel::BeginPlayerTurn()
{
	turn++;
	m_listener->OnPlayerBeginTurn(turn);

	// update statuses

	if (turn == 1)
		DrawToHand(3);

	else
		DrawToHand(1);
}

void CombatModel::PlayCard(Card * card)
{
	if (!m_registeredCards.contains(card))
		return;

	Logger::log(LogLevel::Info, "Player plays {}", card->GetName());

	std::vector<Entity*> targets = { m_enemy.get() };

	EffectContext ctx;
	ctx.observer     = this;
	ctx.entitySource = m_player.get();
	ctx.cardSource   = card;
	ctx.targets      = targets;

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

	if (!capturedIndices.empty())
	{
		playerCaptures++;
		Logger::log(LogLevel::Info, "> Player Capture");

		CaptureEvent evt;
		evt.playedCard = card;
		evt.player     = m_player.get();
		evt.enemy      = m_enemy .get();

		evt.captured.push_back(card);
        for (int idx : capturedIndices)
		{
			evt.captured.push_back(m_table.at(idx));
            m_playerPiles.insert(card, CardPiles::Capture);
        }
		
		// remove captured card on the table
        std::sort(capturedIndices.rbegin(), capturedIndices.rend());
        for (int idx : capturedIndices)
            m_table.erase(m_table.begin() + idx);
        
        if (m_table.empty())
		{
            ++scopaCount;
	        Logger::log(LogLevel::Info, "> Player Scopa");

        }

		m_playerPiles.remove(card, CardPiles::Hand);
		m_listener->OnCardsCaptured(evt.captured);
    }
	else
	{
		Logger::log(LogLevel::Info, "> Player added a card to table");
        m_table.push_back(card);
		m_playerPiles.remove(card, CardPiles::Hand);
		
		m_listener->OnCardPlacedOnTable(card);
    }
	
	EndPlayerTurn();
}

void CombatModel::EndPlayerTurn()
{
	// Resolve effects and statuses
	m_listener->OnPlayerEndTurn();
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

void CombatModel::BeginEnemyTurn()
{
	m_listener->OnEnemyBeginTurn();

	if (turn == 1)
	{
		std::vector<Card *> drawnCards = m_enemyPiles.draw(3);
		m_enemyPiles.insert(drawnCards, CardPiles::Hand);
	}
	else
	{
		std::vector<Card *> drawnCards = m_enemyPiles.draw(1);
		m_enemyPiles.insert(drawnCards, CardPiles::Hand);
	}

	// update statuses
	// play enemy turn here
	m_enemy->planTurn(m_enemyPiles.hand, m_table);
	auto card = m_enemy->executeTurn(m_enemyPiles.hand);

	if (!card)
	{
		Logger::log(LogLevel::Info, "Enemy has no card to play");
		m_listener->OnEnemyEndTurn();
		return;
	}

	std::vector<Entity*> targets = { m_player.get() };

	if (m_enemy->hasStatus("Confusion"))
	{
		Logger::log(LogLevel::Info, "[enemy is Confused and targets itself!]");
		targets = { m_enemy.get() };
	}

	EffectContext ctx;
	ctx.observer     = this;
	ctx.entitySource = m_enemy.get();
	ctx.cardSource   = card;
	ctx.targets      = targets;

	for (auto & effect : card->GetEffects())
	{
		std::cout << " [Effect]: " << effect->getDescription() << std::endl;
		effect->apply(ctx);
	}

	int targetVal = card->GetNumericValue();

	auto capturedIndices = FindCaptureIndices(card, targetVal);

	if (!capturedIndices.empty())
	{
		enemyCaptures++;
        Logger::log(LogLevel::Info, "> Enemy Capture");

		CaptureEvent evt;
		evt.playedCard = card;
		evt.player     = m_player.get();
		evt.enemy      = m_enemy .get();

		evt.captured.push_back(card);
        for (int idx : capturedIndices)
		{
			evt.captured.push_back(m_table.at(idx));
            m_enemyPiles.insert(card, CardPiles::Capture);
        }
		
		// remove captured card on the table
        std::sort(capturedIndices.rbegin(), capturedIndices.rend());
        for (int idx : capturedIndices)
            m_table.erase(m_table.begin() + idx);
        
        if (m_table.empty())
		{
			Logger::log(LogLevel::Info, "Enemy Scopa");
        }

		// card already removed from hand with execute turn
		m_listener->OnCardsCaptured(evt.captured);
    }
	else
	{
		Logger::log(LogLevel::Info, "[Enemy] Added card to table");
        m_table.push_back(card);
		// card already removed from hand with execute turn
		
		m_listener->OnCardPlacedOnTable(card);
    }
	
	m_listener->OnEnemyEndTurn();
}

void CombatModel::DrawToTable(int count)
{
	std::vector<Card *> drawnCards = m_playerPiles.draw(count);
	if (drawnCards.empty()) return;

	m_table.insert(m_table.end(), drawnCards.begin(), drawnCards.end());
	m_listener->OnCardsDrawnToTable(drawnCards);
}

void CombatModel::DrawToHand(int count)
{
	std::vector<Card *> drawnCards = m_playerPiles.draw(count);
	if (drawnCards.empty()) return;

	m_playerPiles.insert(drawnCards, CardPiles::Hand);
	m_listener->OnCardsDrawnToHand(drawnCards);
}

void CombatModel::DrawUntilHandFull()
{
	int count = m_maxHandSize - m_playerPiles.hand.size();
	std::vector<Card *> drawnCards = m_playerPiles.draw(count);
	if (drawnCards.empty()) return;

	m_playerPiles.insert(drawnCards, CardPiles::Hand);
	m_listener->OnCardsDrawnToHand(drawnCards);
}

void CombatModel::Discard(Card * card)
{
	// discard code
}

void CombatModel::Exhaust(Card * card)
{
	// exhaust code
}

void CombatModel::Damage(Entity * dst, uint32_t calculatedDamage, Entity * source, bool isEcho)
{
	// Resolve effects and statuses
	dst->setLastAttackDamage(calculatedDamage);
	dst->takeDamage(calculatedDamage, source, isEcho);

	if(dst == m_player.get())
	{
		m_listener->OnPlayerHealthChange(dst->getHp(), dst->getMaxHp());
	}
	else
	{
		m_listener->OnEnemyHealthChange(dst->getHp(), dst->getMaxHp());
	}

}

void CombatModel::ChangeCardSuit(Card * card, Suit suit)
{
	card->SetSuit(suit);
	m_listener->OnCardUpdate(card);

}

void CombatModel::ChangeCardValue(Card * card, CardValue value)
{
	card->SetValue(value);
	m_listener->OnCardUpdate(card);
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

