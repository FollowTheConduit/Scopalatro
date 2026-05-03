#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>

class Entity;
class Card;

enum class TargetType {
    Enemy,
    Self,
    AllEnemies,
    All
};

struct EffectContext {
    Entity* source;
    std::vector<Entity*> targets;
    Card* cardSource;

    std::vector<Entity*> getTargets(TargetType type) {
        switch (type) {
            case TargetType::Self: return {source};
            case TargetType::Enemy: return targets; 
            case TargetType::AllEnemies: return targets;
            case TargetType::All: {
                std::vector<Entity*> all = targets;
                all.push_back(source);
                return all;
            }
            default: return targets;
        }
    }
};

class Effect {
protected:
    TargetType targetType;
public:
    Effect(TargetType target = TargetType::Enemy) : targetType(target) {}
    virtual ~Effect() = default;
	Effect (Effect && effect) = default;
    virtual void apply(EffectContext& ctx) = 0;
    virtual std::string getDescription() const = 0;
    virtual std::unique_ptr<Effect> clone() const = 0;
};

class DamageEffect : public Effect {
    int amount;
public:
    DamageEffect(int amt, TargetType target = TargetType::Enemy) 
        : Effect(target), amount(amt) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Deal " + std::to_string(amount) + " damage to " + t + ".";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<DamageEffect>(amount, targetType);
    }
};

class HealEffect : public Effect {
    int amount;
public:
    HealEffect(int amt, TargetType target = TargetType::Self) 
        : Effect(target), amount(amt) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Heal " + t + " for " + std::to_string(amount) + " HP.";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<HealEffect>(amount, targetType);
    }
};

class ApplyStatusEffect : public Effect {
    std::string statusName;
    int duration;
public:
    ApplyStatusEffect(std::string name, int dur, TargetType target = TargetType::Enemy) 
        : Effect(target), statusName(name), duration(dur) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Apply " + statusName + " to " + t + " for " + std::to_string(duration) + " turns.";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<ApplyStatusEffect>(statusName, duration, targetType);
    }
};

class BlockEffect : public Effect {
    int amount;
public:
    BlockEffect(int amt, TargetType target = TargetType::Self)
        : Effect(target), amount(amt) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Grant " + std::to_string(amount) + " Shield to " + t + ".";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<BlockEffect>(amount, targetType);
    }
};