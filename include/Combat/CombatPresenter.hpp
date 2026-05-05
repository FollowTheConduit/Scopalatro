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

	void OnCardDropInPlayArea (ObjectID card) override;

	void DebugDrawCard () override;

	ObjectID GenerateObject () override;

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

	ObjectID m_nextID = 0;

	IndexedCardInventory m_cardTable;
	IndexedActorsTable   m_cardActorTable;
};
