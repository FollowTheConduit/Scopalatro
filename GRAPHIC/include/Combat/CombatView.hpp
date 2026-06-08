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
#include <RenderableObjects/CardModel.hpp>
#include <RenderableObjects/HealthbarModel.hpp>

#include <Combat/CombatHelper.hpp>
#include <Combat/CombatViewHelper.hpp>
#include <Combat/CombatViewListener.hpp>
#include <Combat/HandArea.hpp>
#include <Combat/PlayArea.hpp>
#include <TaskManager.hpp>
#include <TaskQueue.hpp>
#include <HoverMap.hpp>



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

	void DamageEntity(ObjectID entityID, int damage);
	void HealEntity  (ObjectID entityID, int heal);

	void SetPrimaryEnemy(ObjectID enemyID);

	void RegisterCardsActor(ObjectID cardID, Suit suit, CardValue value);
	//void RegisterEnemiesActor(IndexedActorsTable enemies);
	CombatView(CombatViewListener * subscriber, TLOT::RenderContext * context, TLOT::Renderer * renderer, TLOT::SceneInspector * inspector);

private:
	std::map<ObjectID, glm::mat4> GetHoveredObjects();
	TaskID GenerateMoveCardTask(ObjectID card, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float(*easingFunction)(float));

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
	PlayArea m_play;


	// Tables
	std::map<ObjectID, CardModel> m_cards;
	//IndexedActorsTable m_enemies;

	std::unique_ptr<HealthbarModel> m_playerHealthbar;
	

	// Events
	TaskManager m_taskManager;
	TaskQueue   m_taskQueue {m_taskManager};
	TaskID      m_dragTask;


	void HoverCard();
	void UnhoverCard();
	void DragCard();
	void DropCard(bool inPlayArea);
};
