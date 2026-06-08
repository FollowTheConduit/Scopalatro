#pragma once

#include <Combat/CombatHelper.hpp>
#include <RenderableObject.hpp>


struct CombatViewListener
{
	virtual void OnCardDropInPlayArea (ObjectID card) = 0;

	virtual void DebugDrawCard () = 0;

	virtual ObjectID GenerateObject () = 0;
};
