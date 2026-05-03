#if 0

#include <Combat/Statuses/Poison.hpp>

#include <Combat/CharacterState.hpp>
#include <Combat/CombatModel.hpp>

void Poison::Solve (BeginTurnContext & ctx, CombatModel & model)
{
	double damage = static_cast<double> (stacks * level);

	DamageContext ctxDamage { ctx.character, DamageSourceType::Status };
	model.DamageEvent (ctxDamage);

	--level;
	if (!level) alive = false;
}

void Poison::Solve (RetriggerPoison & ctx, CombatModel & model)
{
	double damage = static_cast<double> (stacks * level);

	DamageContext ctxDamage { ctx.character, DamageSourceType::Status };
	model.DamageEvent (ctxDamage);

	--level;
	if (!level) alive = false;
}

#endif