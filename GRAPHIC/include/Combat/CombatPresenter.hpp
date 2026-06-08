#pragma once

#include <Combat/CombatModel.hpp>
#include <Combat/CombatView.hpp>
#include <Combat/CombatPresenterHelper.hpp>
#include <Combat/CharacterState.hpp>

#include <memory>
#include <string>
#include <string_view>

#include <Debugger/SceneInspector.hpp>
#include <Renderer/Renderer.hpp>
#include <RenderContext.hpp>

class CombatPresenter: public CombatViewListener, public CombatModelListener
{
public:
	
	void Init();
	void Update();
	void Render();

	void Begin(CombatParams params);

	CombatPresenter(TLOT::RenderContext * context, TLOT::Renderer * renderer, TLOT::SceneInspector * inspector);

private:
	// View events

	void OnCardDropInPlayArea(ObjectID card) override;

	void DebugDrawCard() override;

	ObjectID GenerateObject() override;

	// Model events
	
	void OnMessage(std::string const message) override;

	void OnDiscardToDrawPile(std::vector<ObjectID> cards) override;

	void OnCardsDrawn    (std::vector<ObjectID> cards) override;
	void OnCardsDiscarded(std::vector<ObjectID> cards) override;

	void OnDamage() override;
	void OnDeath () override;

	// Members

	void PreparePlayerTurn();


	TLOT::RenderContext * m_context;
	TLOT::Renderer * m_renderer;

	std::unique_ptr<CombatView>  m_view;
	std::unique_ptr<CombatModel> m_model;

	ObjectID m_nextID = 0;

	IndexedCardTable m_cardTable;

	IndexedEnemyTable m_enemyTable;
	//IndexedActorsTable   m_enemyActorTable;

	std::vector<ObjectID> m_cardsID;
	std::vector<ObjectID> m_enemiesID;
};
