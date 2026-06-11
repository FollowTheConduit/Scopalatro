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


struct DrawArea
{
	TLOT::Transform cardBase;
	std::map<ObjectID, size_t> m_drawPile;
};

class CombatView
{
public:
	void Init();
	void Update();
	void Render();

	void DrawCards(std::vector<ObjectID> cards);
	void DiscardCards(std::vector<ObjectID> cards);
	void ExhaustCards(std::vector<ObjectID> cards);
	void CaptureCards(std::vector<ObjectID> cards);

	void UpdatePlayerHealth(int hp, int maxhp);
	void UpdateEnemyHealth(int hp, int maxhp);

	void UpdateCardModel(ObjectID cardId, CardValue value, Suit suit);

	void SetPrimaryEnemy(ObjectID enemyID);

	void RegisterCard(ObjectID cardID, Suit suit, CardValue value, std::string description);

	void PlaceCardOnTable(ObjectID cardID);

	//void RegisterEnemiesActor(IndexedActorsTable enemies);
	CombatView(CombatViewListener * subscriber, TLOT::RenderContext * context, TLOT::Renderer * renderer, TLOT::SceneInspector * inspector);

private:
	std::map<ObjectID, glm::mat4> GetHoveredObjects();
	TaskID GenerateMoveCardTask(ObjectID card, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float(*easingFunction)(float));

	void GenerateTooltip(ObjectID cardID);
	void DestroyTooltip(ObjectID cardID);

	void RearangeTableCards();

	// Pointers
	CombatViewListener * m_subscriber;

	TLOT::RenderContext * m_context;
	TLOT::Renderer * m_renderer;
	TLOT::SceneInspector * m_inspector;

	// States
	double m_delta;

	bool m_canHover = false;
	bool m_canDrag  = false;

	HoverMap m_hoverMap;

	ObjectID m_hoveredCardID = InvalidObject;
	ObjectID m_draggedCardID = InvalidObject;

	ObjectID m_lastHoveredCardID = InvalidObject;
	ObjectID m_lastDraggedCardID = InvalidObject;

	ObjectID m_primaryEnemyID = InvalidObject;

	std::map<ObjectID, EntityMetaData> m_entityData;

	// Positions and sizes
	float m_cardSize = 200.0f;
	float m_handBeginX = 200.0f;

	float m_primaryEnemyX;
	float m_primaryEnemyY;
	float m_primaryEnemyZ;
	float m_enemyScale;

	HandArea m_hand;
	TableArea m_table;
	PlayArea m_play;

	std::map<ObjectID, std::string> m_cardDescriptions;


	// Models
	std::map<ObjectID, CardModel> m_cards;
	std::unique_ptr<HealthbarModel> m_playerHealthbar;
	std::map<ObjectID, std::unique_ptr<ToolTipModel>> m_activeTooltips;

	// Events
	TaskManager m_taskManager;
	TaskQueue   m_taskQueue {m_taskManager};
	TaskID      m_dragTask;


	void HoverCard();
	void UnhoverCard();
	void DragCard();
	void DropCard(bool inPlayArea);
};
