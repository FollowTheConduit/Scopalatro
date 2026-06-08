#if 0

#pragma once

#include <Combat/CharacterState.hpp>
#include <Combat/CombatModel.hpp>
#include <Combat/Status.hpp>


struct Phoenix : public Status<DeathContext>
{
	void Solve (DeathContext & ctx, CombatModel & model);
};

#endif