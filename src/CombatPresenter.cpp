#include <Combat/CombatPresenter.hpp>

#include <Renderer/Renderables/Sprite.hpp>

#include <EnemyName.hpp>

#include <CardModel.hpp>

using namespace TLOT;

//static std::map<std::string, SpriteAnimation> GetEnemyAnimations (std::string enemyName, IRenderer & manager)
//{
//	static std::map<std::string, SpriteAnimation> const badaliscAnimations = {
//		{"idle", SpriteAnimation { GetTexturesFromKeys ({"enemy_badalisc_idle_0"}, manager) }}
//	};
//
//	if (enemyName == BADALISC)
//		return badaliscAnimations;
//
//	return {};
//}

//static Sprite CreateEnemyActor (std::string name, IRenderer & renderer)
//{
//	Logger::log (LogLevel::Info, "Generating EnemyActor for {} enemy", name);
//
//	Mesh enemyMesh = AssetManager::GetQuadMesh ();
//
//	auto animations = GetEnemyAnimations (name, renderer);
//	Model base = manager.Create (enemyMesh, {});
//	auto sprite = Sprite {base, assetManager};
//
//	for (auto & animation : animations)
//	{
//		sprite.RegisterAnimation (animation.first, animation.second);
//	}
//
//	sprite.SetAnimation ("idle");
//	sprite.ForceUpdate ();
//	return sprite;
//}

CombatView * CombatPresenter::GetView ()
{
	return m_view.get ();
}

void CombatPresenter::PreparePlayerTurn ()
{
	m_model->ShuffleDeck ();
	m_model->Draw (m_model->EffectiveHandSize ());
}

CombatPresenter::CombatPresenter (TLOT::RenderContext & context, TLOT::Camera & camera, TLOT::IRenderer & sceneRenderer, TLOT::IRenderer & uiRenderer, CombatParams params)
{
	for (auto & card : params.playerCards)
	{
		m_cardsID.push_back (m_nextID);
		m_cardTable.emplace (m_nextID, card);
		m_cardActorTable.emplace (m_nextID++, CardModel::Create (card->GetSuit (), card->GetValue (), uiRenderer));
	}


	//for (auto & enemy : params.enemies)
	//{
	//	m_enemiesID.push_back (m_nextID);
	//	m_enemyTable.emplace (m_nextID, enemy);
	//	m_enemyActorTable.emplace (m_nextID, CreateEnemyActor (enemy->name, sceneManager));
	//}

	m_model = std::make_unique<CombatModel> (this, m_cardTable);
	m_view  = std::make_unique<CombatView>  (this, context, camera, sceneRenderer, uiRenderer);

	m_view->RegisterCardsActor (m_cardActorTable);
	//m_view->RegisterEnemiesActor (m_enemyActorTable);

	//m_view->SetPrimaryEnemy (m_enemiesID[0]);
}

void CombatPresenter::Begin ()
{
	PreparePlayerTurn ();
}

void CombatPresenter::OnCardDropInPlayArea (ObjectID card)
{
	m_model->PlayCard (card);
}

void CombatPresenter::DebugDrawCard ()
{
	m_model->DrawUntilHandFull ();
}

ObjectID CombatPresenter::GenerateObject ()
{
	return m_nextID++;
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


