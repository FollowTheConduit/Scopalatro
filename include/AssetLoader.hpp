#pragma once

#include <AssetManager.hpp>

#include <Card.hpp>

inline void LoadTexture (std::string key, std::string path)
{
	TLOT::AssetManager::Cache (key, TLOT::AssetManager::LoadTexture (path));
}

inline void LoadShaderSource (std::string key, std::string path)
{
	TLOT::AssetManager::Cache (key, TLOT::AssetManager::LoadShaderSource (path));
}

inline void CreateShaderCollection (std::string collectionKey, std::string vertexKey, std::string fragmentKey)
{
	TLOT::AssetManager::Cache (collectionKey, TLOT::AssetManager::CreateShaderCollection (TLOT::AssetManager::Cache (vertexKey), TLOT::AssetManager::Cache (fragmentKey)));
}

inline void LoadAll ()
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

	LoadShaderSource ("shader_source_vertex_default"  , "data/assets/shaders/default.vertex");
	LoadShaderSource ("shader_source_vertex_ui"       , "data/assets/shaders/ui.vertex");
	LoadShaderSource ("shader_source_fragment_default", "data/assets/shaders/default.fragment");
	
	CreateShaderCollection ("shader_program_default", "shader_source_vertex_default", "shader_source_fragment_default");
	CreateShaderCollection ("shader_program_card"   , "shader_source_vertex_ui",      "shader_source_fragment_default");
}

