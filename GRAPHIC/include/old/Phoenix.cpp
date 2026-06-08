#include <Combat/Statuses/Phoenix.hpp>

#include <Combat/CharacterState.hpp>
#include <Combat/CombatModel.hpp>

void Phoenix::Solve (DeathContext & ctx, CombatModel & model)
{
	ctx.source.victim.HP = ctx.source.victim.maxHP / 2;

	alive = false;
}
