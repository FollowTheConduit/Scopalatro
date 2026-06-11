
#include <RenderableObjects/CardModel.hpp>

#include <Combat/CombatPresenter.hpp>

using namespace TLOT;

void CombatPresenter::PreparePlayerTurn()
{
	
}

CombatPresenter::CombatPresenter(TLOT::RenderContext * context, TLOT::Renderer * renderer, SceneInspector * inspector)
{
	m_model = std::make_unique<CombatModel>(this);
	m_view  = std::make_unique<CombatView> (this, context, renderer, inspector);
}

void CombatPresenter::Init()
{

}

void CombatPresenter::Update()
{
	m_view->Update();
}

void CombatPresenter::Render()
{
	m_view->Render();
}

void CombatPresenter::Begin(CombatParams params)
{
	
	for (auto card : params.playerCards)
	{
		auto cardID = m_nextID++;
		
		m_model->RegisterCard(cardID, card);
		m_view->RegisterCard(cardID, card->GetSuit(), card->GetValue(), m_model->GetCardName(cardID), m_model->GetCardDescription(cardID));
		//m_cardActorTable.emplace(m_nextID++, CardModel{m_renderer, card.GetSuit(), card.GetValue()});
	}
	
	m_view->Init();
	m_model->Init();
}

void CombatPresenter::OnCardDropInPlayArea(ObjectID card)
{
	m_model->PlayCard(card);
}

void CombatPresenter::DebugDrawCard()
{
	m_model->DrawUntilHandFull();
}

ObjectID CombatPresenter::GenerateObject()
{
	return m_nextID++;
}

void CombatPresenter::OnMessage(std::string const message)
{
	Logger::log(LogLevel::Info, "Message : {}", message);
}

void CombatPresenter::OnDiscardToDrawPile(std::vector<ObjectID> cards)
{

}

void CombatPresenter::OnCardsDrawn(std::vector<ObjectID> cards)
{
	m_view->DrawCards(cards);
}

void CombatPresenter::OnCardsDiscarded(std::vector<ObjectID> cards)
{
	m_view->DiscardCards(cards);
}

void CombatPresenter::OnPlayerHealthChange(int newHP, int newMaxHP)
{
	m_view->UpdatePlayerHealth(newHP, newMaxHP);
}

void CombatPresenter::OnEnemyHealthChange(int newHP, int newMaxHP)
{
	m_view->UpdateEnemyHealth(newHP, newMaxHP);
}

void CombatPresenter::OnPlayerDeath()
{
	//exit(-1);
}

void CombatPresenter::OnEnemyDeath()
{
	m_state = GameState::Shop;
}

void CombatPresenter::OnCardUpdate(ObjectID cardID, CardValue value, Suit suit, std::string name, std::string description)
{
	m_view->UpdateCardModel(cardID, value, suit);
	m_view->UpdateCardDescription(cardID, name, description);
}

void CombatPresenter::OnCardPlacedOnTable(ObjectID cardID)
{
	m_view->PlaceCardOnTable(cardID);
}

void CombatPresenter::OnCardDrawToTable(std::vector<ObjectID> cards)
{
	m_view->DrawCardsToTable(cards);
}

void CombatPresenter::OnCardsCaptured(std::vector<ObjectID> cards)
{
	m_view->CaptureCards(cards);
}

void CombatPresenter::OnPlayerBeginTurn(int turnCount)
{
	m_view->EnableUserInput();
	m_view->DisplayTurnNumber(turnCount);
}

void CombatPresenter::OnPlayerEndTurn()
{
	m_view->DisableUserInput();
	m_model->BeginEnemyTurn();
}

void CombatPresenter::OnEnemyBeginTurn()
{

}

void CombatPresenter::OnEnemyEndTurn()
{
	m_model->BeginPlayerTurn();
}
