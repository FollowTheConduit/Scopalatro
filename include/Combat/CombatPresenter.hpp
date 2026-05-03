#pragma once

#include <Combat/CombatModel.hpp>
#include <Combat/CombatView.hpp>
#include <Combat/CombatPresenterHelper.hpp>
#include <Combat/CharacterState.hpp>

#include <memory>
#include <string>
#include <string_view>

#include <modules/Renderable.hpp>

#include <opengl/RenderContext.hpp>

#include <modules/Camera.hpp>

class CombatPresenter: public CombatViewListener, public CombatModelListener
{
public:
	CombatView * GetView ();

	CombatPresenter (TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::RenderableManager & sceneManager, TLOT::RenderableManager & uiManager, CombatParams params);

	void Begin ();

private:
	// View events

	void OnCardHoverStart (ObjectID card) override;
	void OnCardHoverStop  (ObjectID card) override;

	void OnCardPress  (ObjectID card) override;

	void OnCardDrag  (ObjectID card) override;
	void OnCardDrop  (ObjectID card) override;

	void OnEnemyHover  (ObjectID enemy) override;
	void OnEnemyPress  (ObjectID enemy) override;

	void DebugDrawCard () override;

	// Model events
	
	void OnMessage (std::string const message) override;

	void OnDiscardToDrawPile (std::vector<ObjectID> cards) override;

	void OnCardsDrawn     (std::vector<ObjectID> cards) override;
	void OnCardsDiscarded (std::vector<ObjectID> cards) override;

	void OnDamage () override;
	void OnDeath  () override;

	// Members

	void PreparePlayerTurn ();

	std::unique_ptr<CombatView>  m_view;
	std::unique_ptr<CombatModel> m_model;

	std::map<ObjectID, DisplayEventID> m_descriptions;
	
	ObjectID m_cardTemp;
	bool m_waitingForTarget;

	IndexedCardInventory m_cardTable;
	IndexedActorsTable   m_cardActorTable;
};
