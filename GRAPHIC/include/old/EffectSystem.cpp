#include "EffectSystem.hpp"
#include "Entity.hpp"

void DamageEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for (auto* target : targets) {
        if(target) target->takeDamage(amount);
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
        if (target) target->addStatus(statusName, duration);
    }
}

void BlockEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for (auto* target : targets) {
        if (target) target->gainBlock(amount);
    }
}