#pragma once

#include <map>

#include <MVPHelper.hpp>

enum class HoverType
{
	None    = -1,
	Instant =  0,
	Short   =  1,
	Medium  =  2,
	Long    =  3,
};

class HoverMap
{
public:
	void Hover(ObjectID id, double deltaTime);
	HoverType GetHover(ObjectID id);

	void Update();

private:
	struct HoverCount
	{
		double frameCount = 0.0; // default
		bool hoveredThisFrame = false;
	};

	std::map<ObjectID, HoverCount> m_hoverCount;
};
