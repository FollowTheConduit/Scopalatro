#include <Combat/CombatPresenter.hpp>
#include <modules/Sprite.hpp>

#include <EnemyName.hpp>

using namespace TLOT;

static std::vector<ResourceHandle> GetTexturesFromKeys (std::vector<std::string> textureKeys, RenderableManager & manager)
{
	std::vector<ResourceHandle> textureIDs;

	for (auto & key : textureKeys)
	{
		auto id = manager.GetAssetManager ().GetTextureID (key);
		textureIDs.push_back (id);
		manager.GetRenderer ().RegisterTexture (id, manager.GetAssetManager ().GetTexture (id));
	}

	return textureIDs;
}

static std::map<std::string, SpriteAnimation> GetEnemyAnimations (std::string enemyName, RenderableManager & manager)
{
	static std::map<std::string, SpriteAnimation> const badaliscAnimations = {
		{"idle", SpriteAnimation { GetTexturesFromKeys ({"enemy_badalisc_idle_0"}, manager) }}
	};

	if (enemyName == BADALISC)
		return badaliscAnimations;

	return {};
}

static Renderable CreateCardActor (Suit suit, CardValue value, Transform transform, RenderableManager & manager)
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

static Sprite CreateEnemyActor (std::string name, RenderableManager & manager)
{
	Logger::log (LogLevel::Info, "Generating EnemyActor for {} enemy", name);
	auto & assetManager = manager.GetAssetManager ();

	Mesh enemyMesh = assetManager.GetMesh (assetManager.GetQuadMeshID ());

	auto animations = GetEnemyAnimations (name, manager);
	Renderable base = manager.Create (enemyMesh, {});
	auto sprite = Sprite {base, assetManager};

	for (auto & animation : animations)
	{
		sprite.RegisterAnimation (animation.first, animation.second);
	}

	//sprite.ClearDiffuseTextures (manager.GetAssetManager ());

	sprite.SetAnimation ("idle");
	sprite.ForceUpdate ();
	return sprite;
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
	Renderer & uiRenderer = uiManager.GetRenderer ();
	AssetManager & assetManager = uiManager.GetAssetManager ();

	
	for (auto & card : params.playerCards)
	{
		m_cardTable.emplace (m_nextID, card);
		m_cardActorTable.emplace (m_nextID++, CreateCardActor (card->GetSuit (), card->GetValue (), Transform {}, uiManager));
	}

	for (auto & enemy : params.enemies)
	{
		m_enemyTable.emplace (m_nextID, enemy);
		m_enemyActorTable.emplace (m_nextID, CreateEnemyActor (enemy->name, sceneManager));
	}

	m_model = std::make_unique<CombatModel> (this, m_cardTable);
	m_view  = std::make_unique<CombatView>  (this, context, camera, sceneManager, uiManager);

	m_view->RegisterCardsActor (m_cardActorTable);
	m_view->RegisterEnemiesActor (m_enemyActorTable);
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


