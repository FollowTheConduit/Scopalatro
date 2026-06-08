#pragma once

#include <map>
#include <string>

#include <Core/ResourceHandle.hpp>
#include <Renderer/Renderer.hpp>
#include <RenderContext.hpp>
#include <RenderableObject.hpp>
#include <Debugger/SceneInspector.hpp>

#include <Combat/CombatPresenter.hpp>

class Game
{
public:

	Game(TLOT::RenderContext * context, TLOT::Renderer * renderer, TLOT::SceneInspector * inspector, std::shared_ptr<TLOT::VertexTemplate> vertexTemplate);

	void Setup();

	void Update(double deltaTime);
	void Render();

private:
	void InitPlayerDeck();
	CombatParams GenerateCombatParameters();


	void SetupGeometry();
	void SetupTextures();
	void SetupShaders();
	void SetupMaterials();
	void SetupTechniques();


	TLOT::RenderContext * m_context;
	TLOT::Renderer * m_renderer;
	TLOT::SceneInspector * m_inspector;

	std::shared_ptr<TLOT::VertexTemplate> m_vertexTemplate;


	//std::vector<TLOT::IObject *> m_objects;

	// Player Persistent Data
	float m_playerHP;
	float m_playerMaxHP;
	std::vector<std::unique_ptr<Card>> m_playerDeck;



	CombatPresenter m_combat;
};
