#pragma once

#include <AssetManager.hpp>
#include <RenderableObject.hpp>
class HealthbarModel : public TLOT::CommonObject
{
public:
	HealthbarModel(TLOT::Renderer * renderer, float health, float poison)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_quad"), TLOT::AssetManager::Cache("material_healthbar")}
	{
		m_material.Set("health", health);
		m_material.Set("poison", poison);
		m_material.Set("fullHealthBar",  renderer->Quad(TLOT::AssetManager::Cache("texture_healthbar_full")));
		m_material.Set("emptyHealthBar", renderer->Quad(TLOT::AssetManager::Cache("texture_healthbar_empty")));
		m_material.Set("fullPoisonBar",  renderer->Quad(TLOT::AssetManager::Cache("texture_poisonbar_full")));
		m_material.Set("emptyPoisonBar", renderer->Quad(TLOT::AssetManager::Cache("texture_poisonbar_empty")));

		auto quad = renderer->Quad(TLOT::AssetManager::Cache("texture_healthbar_full"));

		size_t width = 300;
		size_t height = quad.h * 300 / quad.w;

		SetScale({width, height, 0.});

		m_materialUpdateFlag = true;
	}

	void SetHealth(float health)
	{
		m_material.Set("health", health);
		UpdateMaterial();
	}

	void SetPoison(float poison)
	{
		m_material.Set("poison", poison);
		UpdateMaterial();
	}
};