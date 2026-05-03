#pragma once

#include <stdint.h>
#include <vector>

#include <modules/Renderable.hpp>

#include <Combat/CombatHelper.hpp>

using IndexedActorsTable = std::map<ObjectID, TLOT::Renderable>;

using DisplayEventID = uint64_t;
struct DisplayEvent
{
	double ms;
	std::vector<TLOT::Renderable> renderables;

	void Kill ()
	{
		for (auto & renderable : renderables)
		{
			renderable.Kill ();
		}
	}
};
