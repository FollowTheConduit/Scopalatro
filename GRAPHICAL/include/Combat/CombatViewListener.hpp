#pragma once

#include <Combat/CombatHelper.hpp>
#include <RenderableObject.hpp>
#include <RenderableObjects/CardModel.hpp>


struct CombatViewListener
{
	virtual void OnCardDropInPlayArea (CardModel * actor) = 0;

	virtual void DebugDrawCard () = 0;
};
