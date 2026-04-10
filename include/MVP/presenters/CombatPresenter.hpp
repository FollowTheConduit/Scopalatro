#pragma once

#include <MVP/CombatBase.hpp>
#include <MVP/CombatSubscriber.hpp>
#include <MVP/CombatModelListener.hpp>
#include <MVP/models/CombatModel.hpp>
#include <MVP/views/CombatView.hpp>

#include <memory>
#include <string>
#include <string_view>

struct CombatPresenter: public CombatSubscriber, public CombatModelListener
{
	CombatView * GetView ();


	void GenerateActorsTable ();

	// View events

	void NotifyPlayerHover () override;
	void NotifyPlayerPress () override;

	void NotifyItemHover () override;
	void NotifyItemPress () override;

	void NotifyCardHover () override;
	void NotifyCardPress () override;
	void NotifyCardDrag  () override;
	void NotifyCardDrop  () override;

	void NotifyEnemyHover () override;
	void NotifyEnemyPress () override;

	// Model events
	
	void NotifyCardsDrawn     () override;
	void NotifyCardsExhausted () override;

	void NotifyPVChange       () override;
	void NotifyDeath          () override;

	void NotifyEndCombat      () override;

	std::unique_ptr<CombatView>  m_view;
	std::unique_ptr<CombatModel> m_model;
	
	ObjectID m_cardTemp;
	bool m_waitingForTarget;

	std::map<Card, ObjectID>      m_cardToObject;
	std::map<ObjectID, CardActor> m_objectToCardActor;
};
