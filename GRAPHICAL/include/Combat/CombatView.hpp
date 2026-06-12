#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <RenderContext.hpp>
#include <Debugger/SceneInspector.hpp>
#include <Renderer/Camera.hpp>
#include <Renderer/Renderer.hpp>
#include <Combat/CombatHelper.hpp>
#include <Combat/CombatViewHelper.hpp>
#include <Combat/CombatViewListener.hpp>
#include <Combat/HandArea.hpp>
#include <Combat/CombatTable.hpp>
#include <Combat/PlayArea.hpp>
#include <TaskManager.hpp>
#include <TaskQueue.hpp>
#include <Transform.hpp>
#include <HoverMap.hpp>

#include <RenderableObjects/CardModel.hpp>
#include <RenderableObjects/HealthbarModel.hpp>
#include <RenderableObjects/ToolTipModel.hpp>


class Card;

struct DrawArea
{
	TLOT::Transform cardBase;
	std::map<ObjectID, size_t> m_drawPile;
};

class CombatView
{
public:
	CombatView(
		CombatViewListener * subscriber,
		TLOT::RenderContext * context,
		TLOT::Renderer * renderer,
		TLOT::SceneInspector * inspector
	);
	CardModel * RegisterCard(Card * card);
	void UpdateCard(CardModel * actor, Card * card);

	void Init();
	void Update();
	void Render();

	TaskID DrawCardsToHand(std::vector<CardModel *> cards);
	TaskID DrawCardsToTable(std::vector<CardModel *> cards);
	TaskID DiscardCards(std::vector<CardModel *> cards);
	TaskID CaptureCards(std::vector<CardModel *> cards);
	TaskID ExhaustCards(std::vector<CardModel *> cards);
	TaskID PlaceCardOnTable(CardModel * card);
	TaskID UpdatePlayerHealth(int hp, int maxhp);
	TaskID UpdateEnemyHealth(int hp, int maxhp);
	TaskID DisplayTurnNumber(int turnCount);
	TaskID EnableUserInput();
	TaskID DisableUserInput();



private:
	CombatViewListener * m_subscriber;


	std::vector<CardModel *> GetHoveredCards();
	CardModel * GetHoveredCard();

	TaskID GenerateMoveCardTask(CardModel * card, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float(*easingFunction)(float));

	void StartHoverCard(CardModel * actor);
	void UpdateHover(CardModel * actor, HoverType hoverType, double delta);
	void StopHoverCard(CardModel * actor);

	void StartDraggingCard(CardModel * actor);
	void DragCard(CardModel * actor, glm::vec2 mouse, double deltaTime);
	void DropCard(CardModel * actor, bool shouldPlay);

	TaskID RearangeTableCards();
	TaskID RearangeHandCards();

	// Pointers

	TLOT::RenderContext  * m_context;
	TLOT::Renderer       * m_renderer;
	TLOT::SceneInspector * m_inspector;

	// States
	bool m_canInput = false;
	double m_delta;

	HoverMap m_hoverMap;

	CardModel * m_cardHoveredCurrent = nullptr;
	CardModel * m_cardDraggedCurrent = nullptr;
	CardModel * m_cardHoveredLast = nullptr;
	CardModel * m_cardDraggedLast = nullptr;

	// Positions and sizes
	float m_cardSize = 200.0f;
	float m_handBeginX = 200.0f;

	float m_primaryEnemyX;
	float m_primaryEnemyY;
	float m_primaryEnemyZ;
	float m_enemyScale;

	HandArea  m_hand;
	TableArea m_table;
	PlayArea  m_play;

	// Models
	std::set<std::unique_ptr<CardModel>> m_cards;

	std::unique_ptr<HealthbarModel> m_playerHealthbar;
	std::unique_ptr<HealthbarModel> m_enemyHealthbar;
	
	std::unique_ptr<TLOT::TextObject> m_turnDisplay;

	struct UnavailabiltyCounter
	{
		int counter = 0;
	};

	std::map<CardModel *, UnavailabiltyCounter> m_unavailableCards;


	// Events
	TaskManager m_taskManager;
	TaskQueue   m_taskQueue {m_taskManager};

	TaskID      m_dragTask;
};
