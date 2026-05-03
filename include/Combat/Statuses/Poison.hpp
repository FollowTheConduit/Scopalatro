#if 0

#pragma once
#include <Combat/Status.hpp>


struct Poison : public Status<BeginTurnContext>, public Status<RetriggerPoison>
{
	Poison (CharacterState & target): target {target} {}
	Poison (CharacterState & target, size_t stacks): target {target}, stacks {stacks} {}
	Poison (CharacterState & target, size_t stacks, size_t level): target {target}, stacks {stacks}, level {level} {}

	void Solve (BeginTurnContext & ctx, CombatModel & model) override;
	void Solve (RetriggerPoison & ctx, CombatModel & model) override;

	size_t stacks = 0;
	size_t level = 1;

	CharacterState & target;
};

#endif