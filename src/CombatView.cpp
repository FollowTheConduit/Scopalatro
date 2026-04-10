#include <MVP/views/CombatView.hpp>

// dummy
bool CheckLeftCLick (TLOT::RenderContext::Context ctx)
{
	return true;
}

void CombatView::Update (TLOT::RenderContext::Context ctx, TLOT::Camera const & camera)
{
	if (!m_isActive) return;

	// Card Mouse States
	for (auto const & [ID, actor] : m_cards)
	{
		m_isHoveringCard.Listen ();
		if (actor.RayIntersect2D (ctx.lastMouseX, ctx.lastMouseY, camera.direction) && !m_isHoveringCard.Was ())
		{
			m_isHoveringCard.Cry ();

			m_hoveredCard = ID;
			m_subscriber->NotifyCardHover ();

			m_isHoveringCard.Deafen ();

			break;
		}
		m_isHoveringCard.Deafen ();
	}

	if (!m_isHoveringCard.Is () && m_isHoveringCard.Was ())
	{
		m_subscriber->NotifyCardHoverStop ();
	}

	m_isSelectingCard.Listen ();
	if (m_isHoveringCard.Is () && CheckLeftCLick (ctx) && !m_isSelectingCard.Was ())
	{
		m_isSelectingCard.Cry ();

		m_pressedCard = m_hoveredCard;
		m_subscriber->NotifyCardPress ();
	}
	m_isSelectingCard.Deafen ();


	if (!m_isDraggingCard && CheckLeftCLick (ctx) && !m_isSelectingCard.Is ())
	{
		for (auto const & [ID, actor] : m_cards)
		{
			m_isHoveringCard.Listen ();
			if (actor.RayIntersect2D (ctx.lastMouseX, ctx.lastMouseY, camera.direction))
			{
				m_isDraggingCard = true;
				m_draggedCard = ID;
				m_subscriber->NotifyCardDrag ();

				break;
			}
		}
	}

	if (m_isDraggingCard && !CheckLeftCLick (ctx))
	{
		m_isDraggingCard = false;
		m_droppedCard = m_draggedCard;

		m_subscriber->NotifyCardDrop ();
	}

	// Enemy Mouse States

	for (auto const & [ID, actor] : m_enemies)
	{
		m_isHoveringEnemy.Listen ();
		if (actor.RayIntersect2D (ctx.lastMouseX, ctx.lastMouseY, camera.direction) && !m_isHoveringEnemy.Was ())
		{
			m_isHoveringEnemy.Cry ();

			m_hoveredEnemy = ID;
			m_subscriber->NotifyEnemyHover ();

			m_isHoveringEnemy.Deafen ();

			break;
		}
		m_isHoveringEnemy.Deafen ();
	}

	m_isSelectingEnemy.Listen ();
	if (m_isHoveringEnemy.Is () && CheckLeftCLick (ctx) && !m_isSelectingEnemy.Was ())
	{
		m_isSelectingEnemy.Cry ();

		m_pressedEnemy = m_hoveredEnemy;
		m_subscriber->NotifyEnemyPress ();
	}
	m_isSelectingEnemy.Deafen ();


	for (auto & [eventID, event] : m_displayList)
	{
		event.ms -= ctx.deltaTime;

		if (event.ms <= 0.0)
		{
			MarkEventForDeletion (eventID);
		}
		else
		{
			// Update Event ()
			// like actor positions, etc...
		}
	}
	DeleteMarkedEvents ();

	m_isHoveringCard.Step ();
	m_isSelectingCard.Step ();
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
