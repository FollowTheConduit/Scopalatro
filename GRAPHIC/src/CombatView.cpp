#include <Combat/CombatView.hpp>

#include <Easing.hpp>
#include <InputManager.hpp>

#include <Debugger/DebugRenderer.hpp>


using namespace TLOT;

// Helpers =>

static bool PointInRect(float x, float y, glm::vec4 rect)
{
	bool flag = x > rect.x && x < rect.x + rect.z && y > rect.y && y < rect.y + rect.w;
	//if(flag)
	//{
	//	DebugRenderer::Get().CreateRect(rect, {1.0, 0.0, 0.0}, true);
	//}

	return flag;
}

// Public =>

void CombatView::Init()
{
	float width  = (float)m_context->GetViewport().width;
	float height = (float)m_context->GetViewport().height;

	m_cardSize      = width / 12.8f;
	m_hand.cardSize = m_cardSize;
	m_hand.width    = 10 * m_cardSize;
	m_hand.beginX   =(width - m_hand.width) / 2;

	m_play.actorID = m_subscriber->GenerateObject();
	m_play.beginX  = m_hand.beginX;
	m_play.beginY  = m_cardSize * 2.3;
	m_play.width   = m_hand.width;
	m_play.height  =(height -(m_play.beginY)) * 0.8f;

	m_primaryEnemyX = 0.0f;
	m_primaryEnemyY = 0.4f;
	m_primaryEnemyZ = 3.0f;
	m_enemyScale    = 2.0f;

	//auto table = ModelFactory::CreateSprite(sceneRenderer);
	//table.SetDiffuseTexture(0, AssetManager::Cache("texture_table"));
	//table.SetRotation(glm::radians(glm::vec3 {0.0, 0.0, 0.0}));
	//table.SetPosition(glm::vec3 { 0.0, -0.4, 0.0});
	//table.SetScale(glm::vec3 {3.0, 3.0, 3.0});

	m_playerHealthbar = std::make_unique<HealthbarModel>(m_renderer, 0.0, 0.0);
	m_playerHealthbar->SetPosition({350., 750., 1.});
	m_inspector->RegisterRenderableObject(m_playerHealthbar.get());

	m_taskManager.StartTask(
		m_taskManager.RegisterTask([=, this] (TaskID ID, double progress, double deltaTime) -> TaskResult
		{
			progress += deltaTime * 0.25;

			float curve = easeInOutCirc(std::min(progress, 1.0));
			float currentHealth = glm::mix(0.0, 1.0, curve);

			m_playerHealthbar->SetHealth(currentHealth);

			if (progress >= 1.0)
				return TaskResult::Return;

			return TaskResult::Yield;
		}),
		1.3f
	);
}

void CombatView::Update()
{
	m_delta = m_context->deltaTime;

	bool inPlayArea = false;

	double mouseX, mouseY;
	InputManager::getInstance().getMousePos(mouseX, mouseY);
	mouseY = m_context->GetViewport().height - mouseY;

	double vWidth = m_context->GetViewport().width;
	double vHeight = m_context->GetViewport().height;

	bool leftPressed  = InputManager::getInstance().isMouseButtonPressed (GLFW_MOUSE_BUTTON_1);
	bool leftReleased = InputManager::getInstance().isMouseButtonReleased(GLFW_MOUSE_BUTTON_1);
	


	if(PointInRect(mouseX, mouseY, {m_play.beginX, m_play.beginY, m_play.width, m_play.height}))
		inPlayArea = true;
		
	std::map<ObjectID, glm::mat4> hoveredObjects = GetHoveredObjects();
	for (auto & [id, transform] : hoveredObjects)
		m_hoverMap.Hover(id, m_delta);

	m_hoveredCardID = InvalidObject;
	for (auto & [id, _] : m_hand)
	{
		auto result = m_hoverMap.GetHover(id);

		if (result >= HoverType::Short)
			m_hoveredCardID = id;
	}

	if (leftPressed && m_hoveredCardID != InvalidObject)
	{
		m_draggedCardID = m_hoveredCardID;
		m_hoveredCardID = InvalidObject;
	}
	else if (leftReleased && m_draggedCardID != InvalidObject)
	{
		m_hand.SetDrag(InvalidObject);
		m_hand.SetHover(InvalidObject);

		if (inPlayArea)
		{
			m_subscriber->OnCardDropInPlayArea(m_draggedCardID);
		}
		
		m_draggedCardID = InvalidObject;
		m_hoveredCardID = InvalidObject;
	}

	if (m_hoveredCardID != InvalidObject)
	{
		HoverCard();
	}
	else if (m_hoveredCardID == InvalidObject)
	{
		UnhoverCard();
	}
	
	if (m_draggedCardID != InvalidObject)
	{
		DragCard();
	}
	else if (m_draggedCardID == InvalidObject)
	{
		DropCard(inPlayArea);
	}

	m_hoverMap.Update();
	m_taskManager.Update(m_context->deltaTime);
	m_lastHoveredCardID = m_hoveredCardID;
	m_lastDraggedCardID = m_draggedCardID;

	// Debug stuff	
	DebugRenderer::Get().CreateRect(hoveredObjects[m_hoveredCardID],                             {0.812, 0.729, 0.000});
	DebugRenderer::Get().CreateRect({0.0, 0.0, m_hand.beginX, 30.0},                             {0.600, 0.043, 0.518}, true);
	DebugRenderer::Get().CreateRect({m_hand.beginX + m_hand.width, 0.0, m_hand.beginX, 30.0},    {0.600, 0.043, 0.518}, true);
	DebugRenderer::Get().CreateRect({m_hand.beginX, 0.0, m_hand.width, 30.0},                    {0.055, 0.039, 0.259}, true);

	if(inPlayArea) DebugRenderer::Get().CreateRect({m_play.beginX, m_play.beginY, m_play.width, m_play.height}, {0.812, 0.729, 0.000}, false);
	else           DebugRenderer::Get().CreateRect({m_play.beginX, m_play.beginY, m_play.width, m_play.height}, {0.000, 1.000, 0.220}, false);
	if(InputManager::getInstance().isKeyPressed(GLFW_KEY_D)) m_subscriber->DebugDrawCard();
}

void CombatView::Render()
{
	for (auto & [cardID, actor] : m_cards)
	{
		actor.Render();
	}

	m_playerHealthbar->Render();
}

// Warning: right now, it can't be spammed, otherwise it will have a ugly effect
void CombatView::DrawCards(std::vector<ObjectID> cards)
{
	// if card stack is empty we do nothing
	if(cards.empty())
	{
		return;
	}

	// resets hover and drag states
	m_canHover = false;
	m_canDrag  = false;
	m_hoveredCardID = InvalidObject;
	m_draggedCardID = InvalidObject;

	TaskID switchBackStatesTask = m_taskManager.RegisterTask([this](TaskID ID, double progress, double deltaTime) -> TaskResult
	{
		m_canHover = true;
		m_canDrag  = true;

		Logger::log(LogLevel::Info, "canHover=true; canDrag=true");

		return TaskResult::Return;
	});

	std::map<ObjectID, size_t> cardsAlreadyInHand;
	cardsAlreadyInHand.insert(m_hand.begin(), m_hand.end());

	for(auto & ID : cards)
		m_hand.AddCard(ID);

	TaskID lastTask;
	size_t index = 0;
	for(auto & cardID : cards)
	{
		m_cards.at(cardID).SetPosition({ -100 + index * m_cardSize, m_context->GetViewport().height + m_cardSize * 5, 0.0 });
		lastTask = GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 1.0f, easeInOutCirc);
		m_taskManager.StartTask(lastTask, index * 0.25);

		++index;
	}

	for(auto & [cardID, index] : cardsAlreadyInHand)
	{
		m_taskManager.StartTask(GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 1.0f, easeInOutCirc));
	}

	m_taskManager.StartTaskAfter(switchBackStatesTask, lastTask);
}


void CombatView::DiscardCards(std::vector<ObjectID> cards)
{
	if(cards.empty()) return;

	for(auto cardID : cards)
	{
		m_hand.RemoveCard(cardID);

		auto & card = m_cards.at(cardID);

		auto taskID = GenerateMoveCardTask(cardID, {3000., -500., 0.}, glm::vec3 {m_cardSize}, 3.0f, easeInOutCirc);
		m_taskQueue.PushCancel(cardID, taskID);
	}

	TaskID lastTaskID;
	for(auto & [cardID, index] : m_hand)
	{
		lastTaskID = GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 3.0f, easeInOutCirc);
		m_taskQueue.PushCancel(cardID, lastTaskID);
	}

	m_canHover = true;
	m_canDrag  = true;
	m_hoveredCardID = InvalidObject;
	m_draggedCardID = InvalidObject;
}

void CombatView::ExhaustCards(std::vector<ObjectID> cards)
{

}

void CombatView::CaptureCards(std::vector<ObjectID> cards)
{

}

void DamageEntity(ObjectID entityID, int damage)
{

}

void HealEntity(ObjectID entityID, int heal)
{

}


void CombatView::SetPrimaryEnemy(ObjectID enemyID)
{
	//m_primaryEnemyID = enemyID;
//
	//m_enemies[enemyID].SetPosition({m_primaryEnemyX, m_primaryEnemyY, m_primaryEnemyZ});
}

void CombatView::RegisterCardsActor(ObjectID cardID, Suit suit, CardValue value)
{
	CardModel actor {m_renderer, suit, value};

	actor.SetScale({m_cardSize, m_cardSize, 1.0f});
	actor.SetPosition({-1000.0f, 0.0f, 0.0f});
	actor.SetPivot({0.5f, 0.5f, 0.0f});

	m_cards.emplace(cardID, std::move (std::move(actor)));

	m_inspector->RegisterRenderableObject(&m_cards.at(cardID));
}

//void CombatView::RegisterEnemiesActor(IndexedActorsTable enemies)
//{
//	m_enemies.insert(enemies.begin(), enemies.end());
//
//	for(auto & [ID, _] : enemies)
//	{
//		auto & actor = m_enemies[ID];
//		actor.SetPosition({-1000.0f, 0.0f, 0.0f});
//		actor.SetScale({m_enemyScale, m_enemyScale, 1.0f});
//		actor.SetPivot(glm::vec3 {0., 0., 0.});
//	}
//}

CombatView::CombatView(CombatViewListener * subscriber, TLOT::RenderContext * context, Renderer * renderer, TLOT::SceneInspector * inspector)
	: m_subscriber {subscriber}
	, m_context {context}
	, m_renderer {renderer}
	, m_inspector {inspector}
{

}

// Private =>

std::map<ObjectID, glm::mat4> CombatView::GetHoveredObjects()
{
	auto height = m_context->GetViewport().height;
	
	double mouseX, mouseY;
	InputManager::getInstance().getMousePos(mouseX, mouseY);

	std::map<ObjectID, glm::mat4> hoveredObjects;
	for(auto const & [ID, actor] : m_cards)
	{
		if (actor.GetPosition().z < 0.0 || actor.GetPosition().z > 1.0)
			continue;
		glm::mat4 model = glm::scale(actor.GetTransformMatrix(), glm::vec3 {0.65, 1.0, 1.0});
		
		glm::mat4 invModel = glm::inverse(model);
		glm::vec4 localMouse = invModel * glm::vec4(mouseX, height - mouseY, 0.0f, 1.0f);
		
		if(localMouse.x >= -1.0f && localMouse.x <= 1.0f && 
			localMouse.y >= -1.0f && localMouse.y <= 1.0f)
		{
			hoveredObjects.emplace(ID, model);
		}
	}

	return hoveredObjects;
}

TaskID CombatView::GenerateMoveCardTask(ObjectID cardID, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float(*easingFunction)(float))
{
	auto & card = m_cards.at(cardID);
	glm::vec3 startingPosition = card.GetPosition();
	glm::vec3 startingScale = card.GetScale();

	return m_taskManager.RegisterTask([=, this](TaskID ID, double progress, double deltaTime) -> TaskResult
	{
		progress = progress * speed;
		auto & card = m_cards.at(cardID);

		float curve = easingFunction(std::min(progress, 1.0));
		glm::vec3 currentPosition = glm::mix(startingPosition, targetPosition, curve);
		glm::vec3 currentScale = glm::mix(startingScale, targetScale, curve);
		card.SetPosition(currentPosition);
		card.SetScale(currentScale);

		if(progress >= 1.0)
		{
			return TaskResult::Return;
		}

		return TaskResult::Yield;
	});
}

void CombatView::HoverCard()
{
	// Hover card is valid and different from last frame
	if(m_hoveredCardID != InvalidObject && m_hoveredCardID != m_lastHoveredCardID && m_hoveredCardID != m_draggedCardID && m_draggedCardID == InvalidObject)
	{
		m_hand.SetHover(m_hoveredCardID);
		TaskID taskID;
		for(auto & [cardID, index] : m_hand)
		{
			taskID = GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 5.0f, easeInOutCirc);
			m_taskQueue.PushCancel(cardID, taskID);
		}
	}
}

void CombatView::UnhoverCard()
{
	if(m_hoveredCardID == InvalidObject && m_hoveredCardID != m_lastHoveredCardID && m_draggedCardID == InvalidObject)
	{
		m_hand.SetHover(InvalidObject);

		for(auto & [cardID, index] : m_hand)
		{
			auto taskID = GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 5.0f, easeInOutCirc);
			m_taskQueue.PushCancel(cardID, taskID);
		}
	}
}

void CombatView::DragCard()
{
	if(m_draggedCardID != InvalidObject && m_draggedCardID != m_lastDraggedCardID)
	{
		m_hand.SetDrag(m_draggedCardID);
		m_dragTask = m_taskManager.RegisterTask(
			[&, this, cardID = m_draggedCardID](TaskID ID, double progress, double deltaTime) -> TaskResult
			{
				auto & card = m_cards.at(cardID);

				glm::vec2 cardPos  = card.GetPosition();

				double mouseX, mouseY; InputManager::getInstance().getMousePos(mouseX, mouseY);
				mouseY = m_context->GetViewport().height - mouseY;
				glm::vec2 mousePos {mouseX +(m_cardSize / 2), mouseY +(m_cardSize / 2)};

				float deltaX = std::abs(cardPos.x - mousePos.x);
				float deltaY = std::abs(cardPos.y - mousePos.y);
				float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
				
				float const radius = 0.001f;
				float const speed  = 2.0f;

				if (distance > radius)
				{
					float const trackingSpeed = 12.0f; 
					float factor = 1.0f - std::exp(-trackingSpeed * static_cast<float>(deltaTime));
					
					glm::vec3 targetPosition{mousePos.x, mousePos.y, 1.0f};
					glm::vec3 currentPosition = glm::mix(card.GetPosition(), targetPosition, factor);
					
					card.SetPosition(currentPosition);
				}
				else
				{
					card.SetPosition({mousePos.x, mousePos.y, 1.0f});
				}

				return TaskResult::Yield;
			}
		);
		m_taskQueue.PushCancel(m_draggedCardID, m_dragTask);

		for(auto & [cardID, index] : m_hand)
		{
			if(cardID == m_draggedCardID) continue;

			auto taskID = GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 2.0f, easeInOutCirc);
			m_taskQueue.PushCancel(cardID, taskID);
		}
	}
}

void CombatView::DropCard(bool inPlayArea)
{
	if(m_draggedCardID == InvalidObject && m_draggedCardID != m_lastDraggedCardID)
	{
		m_taskManager.StopTask(m_dragTask);

		m_hand.SetDrag(InvalidObject);
		m_canDrag = false;

		if (!inPlayArea)
		{		
			TaskID lastTaskID;
			for(auto & [cardID, index] : m_hand)
			{
				lastTaskID = GenerateMoveCardTask(cardID, m_hand.GetCardPos(cardID), m_hand.GetCardSize(cardID), 3.0f, easeInOutCirc);
				m_taskQueue.PushCancel(cardID, lastTaskID);
			}

			TaskID switchBackStatesTask = m_taskManager.RegisterTask([this](TaskID ID, double progress, double deltaTime) -> TaskResult
			{
				m_canHover = true;
				m_canDrag  = true;
				m_hoveredCardID = InvalidObject;
				m_draggedCardID = InvalidObject;

				return TaskResult::Return;
			});

			m_taskManager.StartTaskAfter(switchBackStatesTask, lastTaskID);
		}
	}
}
