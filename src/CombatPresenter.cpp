#include <Combat/CombatPresenter.hpp>

using namespace TLOT;

static inline Renderable CreateCardActor (Suit suit, CardValue value, Transform transform, RenderableManager & manager)
{
	auto & assetManager = manager.GetAssetManager ();

	Mesh cardMesh = assetManager.GetMesh (assetManager.GetQuadMeshID ());
	auto backTex = assetManager.GetTextureID ("card_back");

	auto symbol_key = suitToString (suit) + "_" + valueToString (value);
		
	auto cardTex = assetManager.GetTextureID (symbol_key);

	cardMesh.material.color = glm::vec4 {1.0};
	cardMesh.material.diffuseTextures = { backTex, cardTex };

	return manager.Create (cardMesh, transform);
}

CombatView * CombatPresenter::GetView ()
{
	return m_view.get ();
}

void CombatPresenter::PreparePlayerTurn ()
{
	m_model->ShuffleDeck ();
	m_model->Draw (m_model->EffectiveHandSize ());
}

CombatPresenter::CombatPresenter (TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::RenderableManager & sceneManager, TLOT::RenderableManager & uiManager, CombatParams params)
{
	ObjectID nextID = 0;

	Renderer & uiRenderer = uiManager.GetRenderer ();
	AssetManager & assetManager = uiManager.GetAssetManager ();

	
	for (auto & card : params.playerCards)
	{
		m_cardTable.emplace (nextID, card);
		m_cardActorTable.emplace (nextID++, CreateCardActor (card->GetSuit (), card->GetValue (), Transform {}, uiManager));
	}

	m_model = std::make_unique<CombatModel> (this, m_cardTable);
	m_view  = std::make_unique<CombatView>  (this, context, camera, sceneManager, uiManager, m_cardActorTable);
}

void CombatPresenter::Begin ()
{
	PreparePlayerTurn ();
}

void CombatPresenter::OnCardHoverStart (ObjectID card)
{
	auto description = m_model->GetCardDescription (card);

	m_descriptions[card] = m_view->DisplayCardDescription (card, description, -1.0);
}

void CombatPresenter::OnCardHoverStop (ObjectID card)
{
	auto displayEventPos = m_descriptions.find (card);
	if (displayEventPos == m_descriptions.end ()) return;

	DisplayEventID descriptionDisplayID = displayEventPos->second;
	
	m_view->StopDisplayEvent (descriptionDisplayID);
}

void CombatPresenter::OnCardPress (ObjectID card) {}

void CombatPresenter::OnCardDrag (ObjectID card) {}
void CombatPresenter::OnCardDrop (ObjectID card) {}

void CombatPresenter::OnEnemyHover (ObjectID enemy) {}
void CombatPresenter::OnEnemyPress (ObjectID enemy) {}

void CombatPresenter::DebugDrawCard ()
{
	m_model->Draw (1);
}

void CombatPresenter::OnMessage (std::string const message)
{
	Logger::log (LogLevel::Info, "Message : {}", message);
}

void CombatPresenter::OnDiscardToDrawPile (std::vector<ObjectID> cards)
{

}

void CombatPresenter::OnCardsDrawn (std::vector<ObjectID> cards)
{
	m_view->DrawCards (cards);
}

void CombatPresenter::OnCardsDiscarded (std::vector<ObjectID> cards)
{
	m_view->DiscardCards (cards);
}

void CombatPresenter::OnDamage ()
{

}

void CombatPresenter::OnDeath ()
{

}


