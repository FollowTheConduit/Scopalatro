#include "EffectSystem.hpp"
#include "Entity.hpp"
#include "Card.hpp"
#include "ScopaDeck.hpp"

void DamageEffect::apply(EffectContext& ctx) {
	int finalAmount = amount;
	if (ctx.source) {
		finalAmount = ctx.source->calculateDamage(amount);
	}
    auto targets = ctx.getTargets(targetType);
    for (auto* target : targets) {
		if (target) {
			target->setLastAttackDamage(finalAmount);
			target->takeDamage(finalAmount, ctx.source);

			if (ctx.source) {
				for (auto& r : ctx.source->getRelics()) {
					r->onDamageDealt(*ctx.source, *target, finalAmount);
				}
			}
		}
    }
}

void HealEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for (auto* target : targets) {
        if (target) target->heal(amount);
    }
}

void ApplyStatusEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for (auto* target : targets) {
        if (target) target->addStatus(statusName, duration, magnitude);
    }
}

void BlockEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for (auto* target : targets) {
        if (target) target->gainBlock(amount);
    }
}

void DrawCardEffect::apply(EffectContext& ctx) {
	if (ctx.drawCallback) {
		ctx.drawCallback(amount);
	} else {
		std::cout << "Cannot draw cards, no draw callback" << std::endl;
	}
}

void HarvestEffect::apply(EffectContext& ctx) {
	if (!ctx.scorePile) {
		std::cout << "No score pile available to Harvest!" << std::endl;
		return;
	}
	int totalValue = 0;
	for (const auto& card : *ctx.scorePile) {
		if (card) totalValue += card->getNumericValue();
	}
	std::cout << "Harvest! Total capture value: " << totalValue << " damage!" << std::endl;
	auto targets = ctx.getTargets(targetType);
	for (auto* target : targets) {
		if (target) target->takeDamage(totalValue);
	}
}

void BoostTableEffect::apply(EffectContext& ctx) {
	if (!ctx.table) {
		std::cout << "No table available!" << std::endl;
		return;
	}
	std::cout << "Show of Strength! All table cards boosted by +" << boostAmount << "!" << std::endl;
	for (auto& card : *ctx.table) {
		if (card) {
			card->boostValue(boostAmount);
			std::cout << "	" << card->getName() << " -> value " << card->getNumericValue() << std::endl;
		}
	}
}

std::string ChangeTableSuitEffect::getDescription() const {
	return "Change all table cards to " + suitToString(newSuit) + ".";
}

void ChangeTableSuitEffect::apply(EffectContext& ctx) {
	if (!ctx.table) {
		std::cout << "No table available." << std::endl;
		return;
	}
	std::cout << "Demonstration of Intelligence! All table cards become " << suitToString(newSuit) << "!" << std::endl;
	for (auto& card : *ctx.table) {
		if (card) {
			card->setSuit(newSuit);
		}
	}
}

void DoubleDebuffsEffect::apply(EffectContext& ctx) {
	auto targets = ctx.getTargets(targetType);
	for (auto* target : targets) {
		if (target) {
			std::cout << "Incurable Disease! Doubling debuffs on " << target->getName() << "!" << std::endl;
			target->doubleDebuffs();
		}
	}
}

void PoisonExplosionEffect::apply(EffectContext& ctx) {
	auto targets = ctx.getTargets(targetType);
	for (auto* target : targets) {
		if (!target) continue;
		if (target->hasStatus("Poisoned")) {
			auto poison = target->getStatus("Poisoned");
			int burstDmg = poison.magnitude * poison.duration;
			std::cout << "Poison Explosion! " << poison.magnitude << " x " << poison.duration
				<< " turns = " << burstDmg << " burst damage!" << std::endl;
			target->takeDamage(burstDmg);
			target->removeStatus("Poisoned");
		} else {
			std::cout << target->getName() << " has no Poison to detonate." << std::endl;
		}
	}
}