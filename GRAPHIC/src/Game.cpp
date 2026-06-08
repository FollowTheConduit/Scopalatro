#include <Game.hpp>

#include <AssetLoader.hpp>
#include <AssetManager.hpp>

using namespace TLOT;


Game::Game(RenderContext * context, Renderer * renderer, SceneInspector * inspector, std::shared_ptr<VertexTemplate> vertexTemplate)
	: m_context{context}
	, m_renderer{renderer}
	, m_inspector{inspector}
	, m_vertexTemplate{vertexTemplate}
	, m_combat{m_context, renderer, m_inspector}
{
	Setup();

	InitPlayerDeck();

	auto param = GenerateCombatParameters();

	m_combat.Begin(param);
}

void Game::Update(double deltaTime)
{
	m_combat.Update();
}

void Game::Render()
{
	m_combat.Render();
}

void Game::InitPlayerDeck()
{
	for (auto const & suit : { Suit::Bastoni, Suit::Coppe, Suit::Denari, Suit::Spada })
	{
		for (auto const & value : { 
			CardValue::Asso,    CardValue::Due,     CardValue::Tre,
			CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
			CardValue::Sette,   CardValue::Cavallo, CardValue::Fante,
			CardValue::Re
		})
		{
			m_playerDeck.push_back(std::move(std::make_unique<Card>(Card::GetKey (suit, value), "BasicCard", suit, value)));
		}
	}
}

CombatParams Game::GenerateCombatParameters()
{
	CombatParams combatParams;
	combatParams.playerHP    = m_playerHP;
	combatParams.playerMaxHP = m_playerMaxHP;

	for (auto & card : m_playerDeck)
	{
		combatParams.playerCards.emplace_back(card.get());
	}

	combatParams.enemies     = {};
	combatParams.playerItems = {};

	return combatParams;
}

















void Game::Setup()
{
	SetupGeometry();
	SetupTextures();
	SetupShaders();
	SetupMaterials();
	SetupTechniques();
}

void Game::SetupGeometry()
{
	std::vector<float> const cardVertices = {
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f
	};
	
	std::vector<uint32_t> const cardIndices = {0, 1, 2, 3, 4, 5};

	AssetManager::Cache("geometry_quad",
		AssetManager::CreateGeometry(
			cardVertices, cardIndices, m_vertexTemplate
		)
	);
}

void Game::SetupTextures()
{
	// scene
	LoadTexture ("texture_table", "data/assets/textures/table.png");

	// enemies
	LoadTexture ("texture_enemy_badalisc_idle_0", "data/assets/textures/badalisc_placeholder.png");

	// cards
	LoadTexture ("texture_card_back", "data/assets/textures/card_back_normal.png");
	for (auto const & suit : { Suit::Bastoni, Suit::Coppe, Suit::Denari, Suit::Spada })
	{
		for (auto const & value : { 
			CardValue::Asso,    CardValue::Due,     CardValue::Tre,
			CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
			CardValue::Sette,   CardValue::Cavallo, CardValue::Fante,
			CardValue::Re
		})
		{
			std::string key = Card::GetKey (suit, value);
			LoadTexture ("texture_card_" + key, "data/assets/textures/card_" + key + ".png");
		}
	}

	//ui
	LoadTexture("texture_healthbar_full",  "data/assets/textures/ui_health_bar_health_full.png");
	LoadTexture("texture_healthbar_empty", "data/assets/textures/ui_health_bar_health_empty.png");
	LoadTexture("texture_poisonbar_full",  "data/assets/textures/ui_health_bar_poison_full.png");
	LoadTexture("texture_poisonbar_empty", "data/assets/textures/ui_health_bar_poison_empty.png");
}

void Game::SetupShaders()
{
	LoadShaderSource ("shader_source_vertex_common"     , "data/assets/shaders/common.vertex");
	LoadShaderSource ("shader_source_fragment_card"     , "data/assets/shaders/card.fragment");
	LoadShaderSource ("shader_source_fragment_healthbar", "data/assets/shaders/healthBar.fragment");
}

void Game::SetupMaterials()
{
	AssetManager::Cache("material_healthbar",
		AssetManager::CreateMaterialTemplate({
			{"health", Float},
			{"poison", Float},
			{"fullHealthBar" , TexQ},
			{"emptyHealthBar", TexQ},
			{"fullPoisonBar" , TexQ},
			{"emptyPoisonBar", TexQ},
		})
	);

	AssetManager::Cache("material_card",
		AssetManager::CreateMaterialTemplate({
			{"symbol" ,   TexQ},
			{"backgroud", TexQ},
			{"back" ,     TexQ}
		})
	);
}

void Game::SetupTechniques()
{
	ID64_t cardProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_card")
	);

	ID64_t healthbarProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_healthbar")
	);

	Technique cardTechnique;
	cardTechnique.mode     = ProjectionMode::Orthographic;
	cardTechnique.program  = cardProgram;
	cardTechnique.material = AssetManager::Cache("material_card");
	m_renderer->RegisterTechnique(cardTechnique);

	Technique healthBarTechnique;
	healthBarTechnique.mode     = ProjectionMode::Orthographic;
	healthBarTechnique.program  = healthbarProgram;
	healthBarTechnique.material = AssetManager::Cache("material_healthbar");
	m_renderer->RegisterTechnique(healthBarTechnique);
}

//{
//	CardModel obj4 {&renderer, Suit::Denari, CardValue::Quattro};
//	obj4.SetPosition({width/2, height/2, 0.});
//	obj4.SetPivot({.5, .5, 0.});
//	obj4.SetScale({200., 200., 0.0});
//
//	CardModel obj5 {&renderer, Suit::Denari, CardValue::Fante};
//	obj5.SetPosition({width/2 + 100, height/2, 0.});
//	obj5.SetPivot({-.5, -.5, 0.});
//	obj5.SetScale({200., 200., 0.0});
//
//	HealthbarModel obj6 {&renderer, 0.0, 0.0};
//	obj6.SetPosition({200.0, 400., 1.});
//	obj6.SetScale({200., 6., 0.0});
//
//}