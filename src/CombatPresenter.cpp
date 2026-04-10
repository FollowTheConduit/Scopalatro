#include <MVP/presenters/CombatPresenter.hpp>

CombatView * CombatPresenter::GetView ()
{
	return m_view.get ();
}

void CombatPresenter::NotifyPlayerHover ()
{
	// nothing rn
}

void CombatPresenter::NotifyPlayerPress ()
{
	// nothing rn
}

void CombatPresenter::NotifyItemHover ()
{
	// nothing rn
}

void CombatPresenter::NotifyItemPress ()
{
	// nothing rn
}

void CombatPresenter::NotifyCardHover ()
{
	auto card = m_view->GetHoveredCard ();
	auto description = m_model->GetCardDescription (card);

	m_view->DisplayCardDescription (card, description, 0.5);
}

void CombatPresenter::NotifyCardPress ()
{
	// nothing
}

void CombatPresenter::NotifyCardDrop ()
{
	auto card = m_view->GetMovedCard ();

	if (!m_model->CanCardCapture (card))
	{
		//TODO: get string from translation module
		m_view->DisplayActionFailure ("Can't capture cards.");
		return;
	}
	
	if (!m_model->ImplicitTargets (card))
	{
		m_cardTemp = card;
		m_waitingForTarget = true;

		if (m_model->GetSpellTarget (card) == TargetType::Self)
		{
			m_view->DisplayPlayerTargetSelector ();
		}
		else
		{
			m_view->DisplayEnemyTargetSelector ();
		}

		return;
	}

	m_model->ResolveSpell (card);
	m_model->ResolveCapture (card);

	if (m_model->IsExhaust (card))
	{
		m_view->ExhaustCard (card);
	}
	else
	{
		m_view->DiscardCard (card);
	}
}

void CombatPresenter::NotifyEnemyHover ()
{
	// nothing rn
}

void CombatPresenter::NotifyEnemyPress ()
{
	if (m_waitingForTarget && m_model->IsTargetValid (m_cardTemp))
	{
		auto enemy = m_view->GetPressedEnemy ();

		m_model->ResolveSpell (m_cardTemp, enemy);
		m_model->ResolveCapture (m_cardTemp);
	}
}


void CombatPresenter::GenerateActorsTable ()
{
	auto cards = m_model->GetCards ();
	auto enemies = m_model->GetEnemies ();

	ObjectID cardID = 0;
	// TODO: Replace dummy constructor with a real one
	std::map<ObjectID, CardActor> cardActors;
	
	for (auto const & card : cards)
	{
		cardActors.emplace (cardID++, card);
	}


	ObjectID enemyID = 0;
	// TOTO: Same here
	std::map<ObjectID, EnemyActor> enemyActors;
	for (auto const & enemy : enemies)
	{
		enemyActors.emplace (enemyID, enemy);
	}

	m_view->SetActorsTable (cardActors, enemyActors);
}
