#pragma once

#include <AssetManager.hpp>
#include <RenderableObject.hpp>
#include <Card.hpp>

class CardModel : public TLOT::CommonObject
{
public:
	CardModel(TLOT::Renderer * renderer, Suit suit, CardValue value)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_quad"), TLOT::AssetManager::Cache("material_card")}
	{
		std::string symbol_key = "texture_card_" + Card::GetKey(suit, value);
		m_material.Set("symbol", renderer->Quad(TLOT::AssetManager::Cache(symbol_key)));
		m_material.Set("back",   renderer->Quad(TLOT::AssetManager::Cache("texture_card_back")));

		m_materialUpdateFlag = true;
	}
};