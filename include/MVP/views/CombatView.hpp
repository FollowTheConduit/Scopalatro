#pragma once

#include <opengl/RenderContext.hpp>

#include <MVP/CombatBase.hpp>
#include <MVP/CombatSubscriber.hpp>
#include <MVP/BiTemporalState.hpp>

#include <modules/Camera.hpp>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

class CombatView
{
public:
	void SetActorsTable (std::map<ObjectID, CardActor> cardTable, std::map<ObjectID, EnemyActor> enemyTable);

	void Update (TLOT::RenderContext::Context ctx, TLOT::Camera const & camera);

	void StopDisplayEvent (DisplayEventID event);

	DisplayEventID DisplayTurnCount (int turn);
	DisplayEventID DisplayActionFailure (std::string_view reason);
	DisplayEventID DisplayCardDescription (ObjectID card, std::string_view description, double ms);

	void DisplayEnemyTargetSelector  ();
	void DisplayPlayerTargetSelector ();
	void StopDisplayTargetSelector   ();

	void ExhaustCard (ObjectID id);
	void DiscardCard (ObjectID id);


	ObjectID GetHoveredItem () { return 0; } // TODO: implements
	ObjectID GetPressedItem () { return 0; } // TODO: implements

	ObjectID GetHoveredCard () { return m_hoveredCard; }
	ObjectID GetPressedCard () { return m_pressedCard; }
	ObjectID GetMovedCard   () { return m_droppedCard; }

	ObjectID GetHoveredEnemy () { return m_hoveredEnemy; }
	ObjectID GetPressedEnemy () { return m_pressedEnemy; }

private:
	void MarkEventForDeletion (DisplayEventID event);
	void DeleteMarkedEvents ();

	// Pointers
	std::unique_ptr<CombatSubscriber> m_subscriber;
	TLOT::Renderer & m_uiRenderer;

	// Mouse States
	BiTemporalState m_isSelectingCard;
	BiTemporalState m_isHoveringCard;
	bool            m_isDraggingCard = false;

	BiTemporalState m_isSelectingEnemy;
	BiTemporalState m_isHoveringEnemy;

	// View States
	bool m_isActive = true;

	ObjectID m_hoveredCard;
	ObjectID m_pressedCard;
	ObjectID m_draggedCard;
	ObjectID m_droppedCard;
	
	ObjectID m_hoveredEnemy;
	ObjectID m_pressedEnemy;
	
	// Tables
	std::map<ObjectID, CardActor> m_cards;
	std::map<ObjectID, EnemyActor> m_enemies;
	

	// Display Events
	std::map<DisplayEventID, DisplayEvent> m_displayList;
	std::set<DisplayEventID> m_markedEventForDeletion;
};
