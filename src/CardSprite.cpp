
#include <renderables/CardSprite.hpp>

using namespace TLOT;

CardManager::CardManager (RenderableManager & renderableManager):
	m_renderableManager {renderableManager}
{
	m_backNormal = m_renderableManager.GetAssetManager ().LoadTexture ("card_back", "data/assets/textures/card_back_normal.png");

	m_quadMesh = m_renderableManager.GetAssetManager ().GetQuadMeshID ();


	//m_renderableManager.GetRenderer ().RegisterGeometry (m_renderableManager.GetAssetManager ().GetMesh (m_quadMesh));
}

CardSprite CardManager::Create (CardColor color, CardSymbol symbol)
{
	std::string symbol_key = CardSprite::GetKey (color, symbol);
	
	if (m_textures.find (symbol_key) == m_textures.end ())
	{
		TLOT::ResourceHandle texture = m_renderableManager.GetAssetManager ().LoadTexture (symbol_key, "data/assets/textures/card_" + symbol_key + ".png");
		m_textures.emplace (symbol_key, texture);
	}

	
	Mesh quadMesh = m_renderableManager.GetAssetManager ().GetMesh (m_quadMesh);
	quadMesh.material.color = glm::vec4 {1.0};
	quadMesh.material.diffuseTextures = { m_backNormal, m_textures[symbol_key] };
	
	auto renderable = m_renderableManager.Create (quadMesh, Transform {});

	return CardSprite {color, symbol, renderable};
}