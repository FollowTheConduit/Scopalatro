#include <Combat/CombatView.hpp>

#include <modules/Tweening.hpp>
#include <modules/DebugRenderer.hpp>

#include <core/InputManager.hpp>

#include <core/Utils.hpp>

using namespace TLOT;

EventID CombatView::MoveHandCardEvent (ObjectID cardID, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float (*easingFunction)(float), double ms)
{
	auto & card = m_cards[cardID];

	glm::vec3 startingPosition = card.GetPosition ();
	glm::vec3 startingScale = card.GetScale ();


	return m_eventManager.FireEventAfter ([this, progress = 0.0, cardID, startingPosition, targetPosition, startingScale, targetScale, speed, easingFunction] (EventID ID) mutable -> EventResult
	{
		auto & card = m_cards[cardID];

		float curve = easingFunction (std::min (progress, 1.0));

		glm::vec3 currentPosition = glm::mix (startingPosition, targetPosition, curve);
		glm::vec3 currentScale = glm::mix (startingScale, targetScale, curve);

		card.SetPosition (currentPosition);
		card.SetScale (currentScale);


		if (progress >= 1.0)
		{
			return EventResult::Return;
		}

		progress += m_delta * speed;

		return EventResult::Yield;
	}, ms);
}

void CombatView::MoveHand ()
{
	for (auto & [cardID, index] : m_hand)
	{
		if (m_activeAnimations.find (cardID) != m_activeAnimations.end ())
		{
			m_eventManager.CancelEvent (m_activeAnimations[cardID]);
		}

		if (cardID == m_draggedCard)
		{
			continue;
		}

		m_activeAnimations[cardID] = MoveHandCardEvent (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 2.0f, easeInOutCirc);
	}
}

void CombatView::Update (RenderContext::Context ctx, Camera const & camera)
{
	m_delta = ctx.deltaTime;


	if (InputManager::getInstance ().isKeyPressed (GLFW_KEY_D))
	{
		m_subscriber->DebugDrawCard ();
	}


	std::map<ObjectID, glm::mat4> hoveredObjects;
	for (auto const & [ID, actor] : m_cards)
	{
		glm::mat4 model = glm::scale (actor.GetModelMatrix(), glm::vec3 {0.65, 1.0, 1.0});
		
		glm::mat4 invModel = glm::inverse (model);
		glm::vec4 localMouse = invModel * glm::vec4(ctx.mouseX, m_context.GetViewport ().height - ctx.mouseY, 0.0f, 1.0f);
		
		if (localMouse.x >= -1.0f && localMouse.x <= 1.0f && 
			localMouse.y >= -1.0f && localMouse.y <= 1.0f)
		{
			hoveredObjects.emplace (ID, model);
		}
	}

	ObjectID hoveredCard = (ObjectID)-1;

	for (auto const & [ID, _] : m_hand)
	{
		if (hoveredObjects.find (ID) != hoveredObjects.end () && m_canHover)
		{
			hoveredCard = ID;
		}
	}

	if (InputManager::getInstance ().isMouseButtonDown (GLFW_MOUSE_BUTTON_1) && m_draggedCard == (ObjectID)-1 && hoveredCard != -1)
	{
		auto & draggedCard = m_cards[hoveredCard];
		m_hand.SetDrag (hoveredCard);
		m_draggedCard = hoveredCard;
		m_canHover = false;

		m_eventManager.CancelEvent (m_activeAnimations[hoveredCard]);
		m_activeAnimations.erase (hoveredCard);
		m_dragCardAnimation = m_eventManager.FireEvent ([&] (EventID ID) mutable -> EventResult
		{
			auto & card = m_cards[m_draggedCard];

			float progress = 1.0f;
			float curve = easeInOutCirc (std::min (progress, 1.0f));

			glm::vec3 currentPosition = glm::mix (card.GetPosition (), {ctx.mouseX, m_context.GetViewport ().height - ctx.mouseY, 1.0f}, curve);
			card.SetPosition (currentPosition);


			return EventResult::Yield;
		});

		MoveHand ();
	}
	else if (!InputManager::getInstance ().isMouseButtonDown (GLFW_MOUSE_BUTTON_1) && m_draggedCard != -1)
	{
		m_eventManager.CancelEvent (m_dragCardAnimation);
		m_hand.SetDrag ((ObjectID)-1);
		m_draggedCard = (ObjectID)-1;

		EventID lastEvent;
		
		for (auto & [ID, index] : m_hand)
		{
			auto & card = m_cards [ID];

			if (m_activeAnimations.find (ID) != m_activeAnimations.end ())
			{
				m_eventManager.CancelEvent (m_activeAnimations[ID]);
			}

			lastEvent = m_activeAnimations[ID] = MoveHandCardEvent (ID, m_hand.GetCardPos (ID), m_hand.GetCardSize (ID), 5.0f, easeInOutCirc);
		}

		m_eventManager.FireEventAfterAnother ([this] (EventID ID) -> EventResult
		{
			m_canHover = true;
			return EventResult::Return;
		}, lastEvent);
	}

	if (hoveredCard != -1 && m_hoveredCardLast != hoveredCard)
	{
		m_hand.SetHover (hoveredCard);
		
		MoveHand ();
	}
	else if (hoveredCard == -1 && m_hoveredCardLast != hoveredCard)
	{
		m_hand.SetHover ((ObjectID)-1);

		MoveHand ();
	}

	DebugRenderer::Get ().CreateRect (hoveredObjects[hoveredCard], {0.812, 0.729, 0.0});
	DebugRenderer::Get ().CreateRect ({0.0, 0.0, m_hand.beginX, 30.0},                          {0.600, 0.043, 0.518}, true);
	DebugRenderer::Get ().CreateRect ({m_hand.beginX + m_hand.width, 0.0, m_hand.beginX, 30.0}, {0.600, 0.043, 0.518}, true);
	DebugRenderer::Get ().CreateRect ({m_hand.beginX, 0.0, m_hand.width, 30.0},                 {0.055, 0.039, 0.259}, true);

	m_eventManager.Poll (ctx.deltaTime);

	m_hoveredCardLast = hoveredCard;
}

DisplayEventID CombatView::DisplayTurnCount (int turn)
{
	// Create a renderable
	// push it to the renderer
	// emplace the renderable to a display set ==> generate an Event (id to renderable)
	// return the generated Event

	return DisplayEventID {};
}

DisplayEventID CombatView::DisplayActionFailure (std::string_view reason)
{
	// Create a renderable
	// push it to the renderer
	// emplace the renderable to a display set ==> generate an Event (id to renderable)
	// return the generated Event

	return DisplayEventID {};
}

DisplayEventID CombatView::DisplayCardDescription (ObjectID card, std::string_view description, double ms)
{
	// Create a renderable
	// push it to the renderer
	// emplace the renderable to a display set ==> generate an Event (id to renderable)
	// return the generated Event

	return DisplayEventID {};
}

void CombatView::StopDisplayEvent (DisplayEventID event)
{
	assert (m_displayList.find (event) == m_displayList.end ());

	MarkEventForDeletion (event);
	DeleteMarkedEvents ();
}

void CombatView::DisplayEnemyTargetSelector ()
{

}

void CombatView::DisplayPlayerTargetSelector ()
{

}

void CombatView::StopDisplayTargetSelector   ()
{

}

void CombatView::MarkEventForDeletion (DisplayEventID event)
{
	m_markedEventForDeletion.emplace (event);
}

void CombatView::DeleteMarkedEvents ()
{
	for (auto const & eventID : m_markedEventForDeletion)
	{
		DisplayEvent & event = m_displayList.at (eventID);

		event.Kill ();

		m_displayList.erase (eventID);
	}
}

CombatView::CombatView (CombatViewListener * subscriber, TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::RenderableManager & sceneManager, TLOT::RenderableManager & uiManager, IndexedActorsTable cards):
	m_subscriber {subscriber},
	m_sceneManager {sceneManager},
	m_uiManager {uiManager},
	m_camera {camera},
	m_context {context},
	m_cards {cards}
{

	m_cardSize = context.GetViewport ().width / 12.8f;
	m_hand.cardSize = m_cardSize;
	m_hand.width = 10 * m_cardSize;
	m_hand.beginX    = (m_context.GetViewport ().width - m_hand.width) / 2;

	for (auto & [ID, actor] : m_cards)
	{
		actor.SetScale ({m_cardSize, m_cardSize, 1.0f});
		actor.SetPosition ({-1000.0f, 0.0f, 0.0f});
		actor.SetPivot ({0.5f, 0.5f, 0.0f});
	}
}

void CombatView::DrawCards (std::vector<ObjectID> cards)
{
	// if card stack is empty we do nothing
	if (cards.empty ())
	{
		return;
	}

	m_canHover = false;

	for (auto & ID : cards)
	{
		m_hand.AddCard (ID);
	}

	size_t index = 0;
	float timeStamp;
	EventID lastEvent;
	for (auto & ID : cards)
	{
		timeStamp = 0.25f * (2 + index);
		
		auto & card = m_cards.at (ID);
		card.SetPosition ({ -100 + index * m_cardSize, m_context.GetViewport ().height + m_cardSize * 2, 0.0 });
		lastEvent = MoveHandCardEvent (ID, m_hand.GetCardPos (ID), m_hand.GetCardSize (ID), 1.0f, easeInOutCirc, 0.25f * (2 + index++));
	}

	for (auto & [ID, index] : m_hand)
	{
		// skip those we are going to add
		if (std::find (cards.begin (), cards.end (), ID) != cards.end ()) continue;

		auto & card = m_cards.at (ID);

		// TODO: add an "after another" version of this function
		MoveHandCardEvent (ID, m_hand.GetCardPos (ID), m_hand.GetCardSize (ID), 3.0f, easeInOutCirc, timeStamp + 0.5f);
	}

	m_eventManager.FireEventAfterAnother ([this] (EventID ID) -> EventResult
	{
		m_canHover = true;

		return EventResult::Return;
	}, lastEvent);
}


void CombatView::DiscardCards (std::vector<ObjectID> cards)
{

}

void CombatView::ExhaustCards (std::vector<ObjectID> cards)
{

}

void CombatView::CaptureCards (std::vector<ObjectID> cards)
{

}

