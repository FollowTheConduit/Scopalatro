#include <Combat/CombatView.hpp>

#include <modules/Tweening.hpp>
#include <modules/DebugRenderer.hpp>

#include <core/InputManager.hpp>

#include <core/Utils.hpp>

using namespace TLOT;

// Helpers =>

static bool PointInRect (float x, float y, glm::vec4 rect)
{
	bool flag = x > rect.x && x < rect.x + rect.z && y > rect.y && y < rect.y + rect.w;
	//if (flag)
	//{
	//	DebugRenderer::Get().CreateRect (rect, {1.0, 0.0, 0.0}, true);
	//}

	return flag;
}

// Public =>

void CombatView::Update (RenderContext::Context ctx, Camera const & camera)
{
	m_delta = ctx.deltaTime;
	std::map<ObjectID, glm::mat4> hoveredObjects = GetHoveredObjects ();
	bool inPlayArea = false;
	double mouseX, mouseY;
	InputManager::getInstance().getMousePos (mouseX, mouseY);
	mouseY = m_context.GetViewport ().height - mouseY;
	double vWidth = m_context.GetViewport ().width;
	double vHeight = m_context.GetViewport ().height;

	if (PointInRect (mouseX, mouseY, {m_play.beginX, m_play.beginY, m_play.width, m_play.height}))
	{
		inPlayArea = true;
	}

	// furthest card on the left is considered hovered
	m_hoveredCardID = InvalidObject;
	for (auto const & [cardID, _] : m_hand)
	{
		if (hoveredObjects.find (cardID) != hoveredObjects.end ()) m_hoveredCardID = cardID;
	}

	if (InputManager::getInstance ().isMouseButtonPressed (GLFW_MOUSE_BUTTON_1) && m_hoveredCardID != InvalidObject)
	{
		m_draggedCardID = m_hoveredCardID;
		m_hoveredCardID = InvalidObject;
	}

	if (InputManager::getInstance ().isMouseButtonReleased (GLFW_MOUSE_BUTTON_1) && m_draggedCardID != InvalidObject)
	{
		m_draggedCardID = InvalidObject;
	}

	if (m_canHover)
	{
		// Hover card is valid and different from last frame
		if (m_hoveredCardID != InvalidObject && m_hoveredCardID != m_lastHoveredCardID)
		{
			m_hand.SetHover (m_hoveredCardID);
			TaskID taskID;
			for (auto & [cardID, index] : m_hand)
			{
				taskID = GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 5.0f, easeInOutCirc);
				m_taskQueue.PushCancel (cardID, taskID);
			}
		}
		// Hover card is invalid and different from last frame
		else if (m_hoveredCardID == InvalidObject && m_hoveredCardID != m_lastHoveredCardID)
		{
			m_hand.SetHover (InvalidObject);

			for (auto & [cardID, index] : m_hand)
			{
				auto taskID = GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 5.0f, easeInOutCirc);
				m_taskQueue.PushCancel (cardID, taskID);
			}
		}
	}

	if (m_canDrag)
	{
		// Dragging card + state change
		if (m_draggedCardID != InvalidObject && m_draggedCardID != m_lastDraggedCardID)
		{		
			m_hand.SetDrag (m_draggedCardID);
			m_canHover = false;

			m_taskQueue.PushCancel (m_draggedCardID, m_taskManager.RegisterTask (
				[&, this] (TaskID ID, double progress, double deltaTime) -> TaskResult
				{
					if (m_draggedCardID == InvalidObject)
					{
						return TaskResult::Return;
					}

					auto & card = m_cards[m_draggedCardID];

					glm::vec2 cardPos  = card.GetPosition ();
					glm::vec2 mousePos {mouseX + (m_cardSize / 2), mouseY + (m_cardSize / 2)};

					float deltaX = std::abs (cardPos.x - mousePos.x);
					float deltaY = std::abs (cardPos.y - mousePos.y);
					float distance = std::sqrt (deltaX * deltaX + deltaY * deltaY);
					
					float const radius = 0.02f;
					float const speed  = 2.0f;

					float time = distance * (deltaTime * speed);
					
					//Logger::log (LogLevel::Info, "{} {}", mousePos.x, mousePos.y);

					if (distance > radius)
					{
						float curve = easeInOutCirc (std::min (time, 1.0f));
						glm::vec3 currentPosition = glm::mix (card.GetPosition (), {mousePos.x, mousePos.y, 1.0f}, curve);
						card.SetPosition (currentPosition);
						//Logger::log (LogLevel::Info, "{} {}", currentPosition.x, currentPosition.y);
					}

					return TaskResult::Yield;
				}
			));

			for (auto & [cardID, index] : m_hand)
			{
				if (cardID == m_draggedCardID) continue;

				auto taskID = GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 2.0f, easeInOutCirc);
				m_taskQueue.PushCancel (cardID, taskID);
			}
		}

		// No longer dragging card + state change
		else if (m_draggedCardID == InvalidObject && m_draggedCardID != m_lastDraggedCardID)
		{
			m_hand.SetDrag (InvalidObject);
			m_canDrag = false;

			if (inPlayArea)
			{
				m_subscriber->OnCardDropInPlayArea (m_lastDraggedCardID);
			}
			else
			{		
				TaskID lastTaskID;
				for (auto & [cardID, index] : m_hand)
				{
					lastTaskID = GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 3.0f, easeInOutCirc);
					m_taskQueue.PushCancel (cardID, lastTaskID);
				}

				TaskID switchBackStatesTask = m_taskManager.RegisterTask ([this] (TaskID ID, double progress, double deltaTime) -> TaskResult
				{
					m_canHover = true;
					m_canDrag  = true;
					m_hoveredCardID = InvalidObject;
					m_draggedCardID = InvalidObject;

					Logger::log (LogLevel::Info, "canHover=true; canDrag=true");

					return TaskResult::Return;
				});

				m_taskManager.StartTaskAfter (switchBackStatesTask, lastTaskID);
			}
		}
	}

	m_taskManager.Update (ctx.deltaTime);
	m_lastHoveredCardID = m_hoveredCardID;
	m_lastDraggedCardID = m_draggedCardID;

	// Debug stuff	
	DebugRenderer::Get ().CreateRect (hoveredObjects[m_hoveredCardID],                             {0.812, 0.729, 0.000});
	DebugRenderer::Get ().CreateRect ({0.0, 0.0, m_hand.beginX, 30.0},                             {0.600, 0.043, 0.518}, true);
	DebugRenderer::Get ().CreateRect ({m_hand.beginX + m_hand.width, 0.0, m_hand.beginX, 30.0},    {0.600, 0.043, 0.518}, true);
	DebugRenderer::Get ().CreateRect ({m_hand.beginX, 0.0, m_hand.width, 30.0},                    {0.055, 0.039, 0.259}, true);

	if (inPlayArea) DebugRenderer::Get ().CreateRect ({m_play.beginX, m_play.beginY, m_play.width, m_play.height}, {0.812, 0.729, 0.000}, false);
	else            DebugRenderer::Get ().CreateRect ({m_play.beginX, m_play.beginY, m_play.width, m_play.height}, {0.000, 1.000, 0.220}, false);
	if (InputManager::getInstance ().isKeyPressed (GLFW_KEY_D)) m_subscriber->DebugDrawCard ();
}

// Warning: right now, it can't be spammed, otherwise it will have a ugly effect
void CombatView::DrawCards (std::vector<ObjectID> cards)
{
	// if card stack is empty we do nothing
	if (cards.empty ())
	{
		return;
	}

	// resets hover and drag states
	m_canHover = false;
	m_canDrag  = false;
	m_hoveredCardID = InvalidObject;
	m_draggedCardID = InvalidObject;

	TaskID switchBackStatesTask = m_taskManager.RegisterTask ([this] (TaskID ID, double progress, double deltaTime) -> TaskResult
	{
		m_canHover = true;
		m_canDrag  = true;

		Logger::log (LogLevel::Info, "canHover=true; canDrag=true");

		return TaskResult::Return;
	});

	std::map<ObjectID, size_t> cardsAlreadyInHand;
	cardsAlreadyInHand.insert (m_hand.begin (), m_hand.end ());

	for (auto & ID : cards)
		m_hand.AddCard (ID);

	TaskID lastTask;
	size_t index = 0;
	for (auto & cardID : cards)
	{
		m_cards.at (cardID).SetPosition ({ -100 + index * m_cardSize, m_context.GetViewport ().height + m_cardSize * 5, 0.0 });
		lastTask = GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 1.0f, easeInOutCirc);
		m_taskManager.StartTask (lastTask, index * 0.25);

		++index;
	}

	for (auto & [cardID, index] : cardsAlreadyInHand)
	{
		m_taskManager.StartTask (GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 1.0f, easeInOutCirc));
	}

	m_taskManager.StartTaskAfter (switchBackStatesTask, lastTask);
}


void CombatView::DiscardCards (std::vector<ObjectID> cards)
{
	if (cards.empty ()) return;

	for (auto cardID : cards)
	{
		m_hand.RemoveCard (cardID);

		auto & card = m_cards[cardID];

		auto taskID = GenerateMoveCardTask (cardID, {0., 0., -1000.}, glm::vec3 {m_cardSize}, 3.0f, easeInOutCirc);
		m_taskQueue.PushCancel (cardID, taskID);
	}

	TaskID lastTaskID;
	for (auto & [cardID, index] : m_hand)
	{
		lastTaskID = GenerateMoveCardTask (cardID, m_hand.GetCardPos (cardID), m_hand.GetCardSize (cardID), 3.0f, easeInOutCirc);
		m_taskQueue.PushCancel (cardID, lastTaskID);
	}

	m_canHover = true;
	m_canDrag  = true;
	m_hoveredCardID = InvalidObject;
	m_draggedCardID = InvalidObject;
}

void CombatView::ExhaustCards (std::vector<ObjectID> cards)
{

}

void CombatView::CaptureCards (std::vector<ObjectID> cards)
{

}

void DamageEntity (ObjectID entityID, int damage)
{

}

void HealEntity   (ObjectID entityID, int heal)
{

}


void CombatView::SetPrimaryEnemy (ObjectID enemyID)
{
	m_primaryEnemyID = enemyID;

	m_enemies[enemyID].SetPosition ({m_primaryEnemyX, m_primaryEnemyY, m_primaryEnemyZ});
}

void CombatView::RegisterCardsActor (IndexedActorsTable cards)
{
	m_cards.insert (cards.begin (), cards.end ());
	
	// otherwise, transform will not be synchronized
	for (auto & [ID, _] : cards)
	{
		auto & actor = m_cards[ID];
		actor.SetScale ({m_cardSize, m_cardSize, 1.0f});
		actor.SetPosition ({-1000.0f, 0.0f, 0.0f});
		actor.SetPivot ({0.5f, 0.5f, 0.0f});
	}
}

void CombatView::RegisterEnemiesActor (IndexedActorsTable enemies)
{
	m_enemies.insert (enemies.begin (), enemies.end ());

	for (auto & [ID, _] : enemies)
	{
		auto & actor = m_enemies[ID];
		actor.SetPosition ({-1000.0f, 0.0f, 0.0f});
		actor.SetScale ({m_enemyScale, m_enemyScale, 1.0f});
		actor.SetPivot (glm::vec3 {0., 0., 0.});
	}
}

CombatView::CombatView (CombatViewListener * subscriber, TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::RenderableManager & sceneManager, TLOT::RenderableManager & uiManager):
	m_subscriber {subscriber},
	m_sceneManager {sceneManager},
	m_uiManager {uiManager},
	m_camera {camera},
	m_context {context}
{

	float width = (float)context.GetViewport ().width;
	float height = (float)context.GetViewport ().height;

	m_cardSize      = width / 12.8f;
	m_hand.cardSize = m_cardSize;
	m_hand.width    = 10 * m_cardSize;
	m_hand.beginX   = (width - m_hand.width) / 2;

	m_play.actorID = m_subscriber->GenerateObject ();
	m_play.beginX  = m_hand.beginX;
	m_play.beginY  = m_cardSize * 2.3;
	m_play.width   = m_hand.width;
	m_play.height  = (height - (m_play.beginY)) * 0.8f;

	m_primaryEnemyX = 0.0f;
	m_primaryEnemyY = 0.4f;
	m_primaryEnemyZ = 3.0f;
	m_enemyScale    = 2.0f;

	auto & assetManager = sceneManager.GetAssetManager ();
	auto quadID = assetManager.GetQuadMeshID ();
	auto quadMesh = assetManager.GetMesh (quadID);
	quadMesh.material.diffuseTextures.push_back (assetManager.GetTextureID ("scene_table"));
	Renderable table = RenderableManager {sceneManager.GetAssetManager (), sceneManager.GetRenderer ()}.Create (quadMesh, {});
	table.SetRotation (glm::radians (glm::vec3 {90.0, 0.0, 0.0}));
	table.SetPosition (glm::vec3 { 0.0, -0.4, 0.0});
	table.SetScale (glm::vec3 {3.0, 3.0, 3.0});
}

// Private =>

std::map<ObjectID, glm::mat4> CombatView::GetHoveredObjects ()
{
	auto height = m_context.GetViewport ().height;
	
	double mouseX, mouseY;
	InputManager::getInstance().getMousePos (mouseX, mouseY);

	std::map<ObjectID, glm::mat4> hoveredObjects;
	for (auto const & [ID, actor] : m_cards)
	{
		glm::mat4 model = glm::scale (actor.GetModelMatrix(), glm::vec3 {0.65, 1.0, 1.0});
		
		glm::mat4 invModel = glm::inverse (model);
		glm::vec4 localMouse = invModel * glm::vec4(mouseX, height - mouseY, 0.0f, 1.0f);
		
		if (localMouse.x >= -1.0f && localMouse.x <= 1.0f && 
			localMouse.y >= -1.0f && localMouse.y <= 1.0f)
		{
			hoveredObjects.emplace (ID, model);
		}
	}

	return hoveredObjects;
}

TaskID CombatView::GenerateMoveCardTask (ObjectID cardID, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float (*easingFunction)(float))
{
	auto & card = m_cards[cardID];
	glm::vec3 startingPosition = card.GetPosition ();
	glm::vec3 startingScale = card.GetScale ();

	return m_taskManager.RegisterTask ([=, this] (TaskID ID, double progress, double deltaTime) -> TaskResult
	{
		progress = progress * speed;
		auto & card = m_cards[cardID];

		float curve = easingFunction (std::min (progress, 1.0));
		glm::vec3 currentPosition = glm::mix (startingPosition, targetPosition, curve);
		glm::vec3 currentScale = glm::mix (startingScale, targetScale, curve);
		card.SetPosition (currentPosition);
		card.SetScale (currentScale);

		if (progress >= 1.0)
		{
			return TaskResult::Return;
		}

		return TaskResult::Yield;
	});
}
