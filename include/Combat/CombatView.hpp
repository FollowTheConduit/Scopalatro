#pragma once

#include <opengl/RenderContext.hpp>

#include <Combat/CombatHelper.hpp>
#include <Combat/CombatViewHelper.hpp>
#include <Combat/CombatViewListener.hpp>
#include <Combat/HandArea.hpp>
#include <Combat/PlayArea.hpp>

#include <BiTemporalState.hpp>
#include <TaskManager.hpp>
#include <TaskQueue.hpp>


#include <modules/Camera.hpp>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

struct DrawArea
{
	TLOT::Transform cardBase;
	std::map<ObjectID, size_t> m_drawPile;
};

class CombatView
{
public:
	void Update (TLOT::RenderContext::Context ctx, TLOT::Camera const & camera);

	void DrawCards (std::vector<ObjectID> cards);
	void DiscardCards (std::vector<ObjectID> cards);
	void ExhaustCards (std::vector<ObjectID> cards);
	void CaptureCards (std::vector<ObjectID> cards);

	CombatView (CombatViewListener * subscriber, TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::RenderableManager & sceneManager, TLOT::RenderableManager & uiManager, IndexedActorsTable cards);

private:
	std::map<ObjectID, glm::mat4> GetHoveredObjects ();
	TaskID GenerateMoveCardTask (ObjectID card, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float (*easingFunction)(float));


	// Pointers
	CombatViewListener * m_subscriber;

	TLOT::RenderContext & m_context;
	TLOT::RenderableManager & m_sceneManager;
	TLOT::RenderableManager & m_uiManager;
	TLOT::Camera & m_camera;

	// States
	double m_delta;

	bool m_canHover = false;
	bool m_canDrag  = false;

	ObjectID m_hoveredCardID = InvalidObject;
	ObjectID m_draggedCardID = InvalidObject;

	ObjectID m_lastHoveredCardID = InvalidObject;
	ObjectID m_lastDraggedCardID = InvalidObject;

	// Positions and sizes
	float m_cardSize = 200.0f;
	float m_handBeginX = 200.0f;

	HandArea m_hand;
	PlayArea m_play;


	// Tables
	IndexedActorsTable m_cards;
	IndexedActorsTable m_enemies;
	

	// Events
	TaskManager m_taskManager;
	TaskQueue   m_taskQueue    {m_taskManager};
	TaskQueue   m_hoverQueue   {m_taskManager};
	TaskQueue   m_discardQueue {m_taskManager};
};
