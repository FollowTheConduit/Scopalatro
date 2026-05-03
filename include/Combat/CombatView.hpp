#pragma once

#include <opengl/RenderContext.hpp>

#include <Combat/CombatHelper.hpp>
#include <Combat/CombatViewHelper.hpp>
#include <Combat/CombatViewListener.hpp>
#include <Combat/HandArea.hpp>

#include <BiTemporalState.hpp>

#include <EventManager.hpp>

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

	void StopDisplayEvent (DisplayEventID event);

	DisplayEventID DisplayTurnCount (int turn);
	DisplayEventID DisplayActionFailure (std::string_view reason);
	DisplayEventID DisplayCardDescription (ObjectID card, std::string_view description, double ms);

	void DisplayEnemyTargetSelector  ();
	void DisplayPlayerTargetSelector ();
	void StopDisplayTargetSelector   ();

	void DrawCards (std::vector<ObjectID> cards);
	void DiscardCards (std::vector<ObjectID> cards);
	void ExhaustCards (std::vector<ObjectID> cards);
	void CaptureCards (std::vector<ObjectID> cards);

	CombatView (CombatViewListener * subscriber, TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::RenderableManager & sceneManager, TLOT::RenderableManager & uiManager, IndexedActorsTable cards);

private:
	void MarkEventForDeletion (DisplayEventID event);
	void DeleteMarkedEvents ();

	void MoveHand ();

	EventID MoveHandCardEvent (ObjectID card, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float (*easingFunction)(float), double ms = -1.0f);

	// Pointers
	CombatViewListener * m_subscriber;

	TLOT::RenderContext & m_context;
	TLOT::RenderableManager & m_sceneManager;
	TLOT::RenderableManager & m_uiManager;
	TLOT::Camera & m_camera;


	// Animations
	std::map<ObjectID, EventID> m_activeAnimations;
	EventID m_dragCardAnimation;
	

	// States
	double m_delta;
	bool m_isActive = true;

	bool m_canHover = false;

	ObjectID m_hoveredCardLast = (ObjectID)-1;
	ObjectID m_draggedCard = (ObjectID)-1;



	// Positions and sizes
	float m_cardSize = 200.0f;
	float m_handBeginX = 200.0f;

	HandArea m_hand;


	// Tables
	IndexedActorsTable m_cards;
	IndexedActorsTable m_enemies;
	

	// Events
	EventManager m_eventManager;

	std::map<DisplayEventID, DisplayEvent> m_displayList;
	std::set<DisplayEventID> m_markedEventForDeletion;
};
