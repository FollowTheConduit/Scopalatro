
#include <RenderableObjects/CardModel.hpp>

#include <Combat/CombatPresenter.hpp>

using namespace TLOT;

void CombatPresenter::PreparePlayerTurn()
{
	m_model->ShuffleDeck();
	m_model->Draw(m_model->EffectiveHandSize());
}

CombatPresenter::CombatPresenter(TLOT::RenderContext * context, TLOT::Renderer * renderer, SceneInspector * inspector)
{
	m_model = std::make_unique<CombatModel>(this);
	m_view  = std::make_unique<CombatView> (this, context, renderer, inspector);
}

void CombatPresenter::Init()
{
	m_view->Init();
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
	Init();
	
	for (auto card : params.playerCards)
	{
		auto cardID = m_nextID++;
		m_cardsID.push_back(cardID);
		m_cardTable.emplace(cardID, card);
		
		m_view->RegisterCardsActor(cardID, card->GetSuit(), card->GetValue());
		m_model->RegisterCard(cardID, card);
		//m_cardActorTable.emplace(m_nextID++, CardModel{m_renderer, card.GetSuit(), card.GetValue()});
	}
	

	PreparePlayerTurn();
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

void CombatPresenter::OnDamage()
{

}

void CombatPresenter::OnDeath()
{

}


