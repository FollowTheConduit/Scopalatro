#pragma once
#include <string>
#include <vector>
#include <memory>
#include "EffectSystem.hpp"
#include "Entity.hpp"
#include "ScopaDeck.hpp"

class Card {
public:
    Card(std::string name, std::string desc, Suit s = Suit::Denari, CardValue v = CardValue::Asso) 
        : name(name), description(desc), suit(s), value(v) {}

    
    void addEffect(std::unique_ptr<Effect> effect) {
        effects.push_back(std::move(effect));
    }

    void addEffect(Effect* effect) {
        effects.push_back(std::unique_ptr<Effect>(effect));
    }

    Suit getSuit() const { return suit; }
    CardValue getValue() const { return value; }
    int getNumericValue() const { return static_cast<int>(value); }

    void play(Entity& source, std::vector<Entity*> targets) {
        std::cout << "\n> Playing card: " << name << std::endl;

        EffectContext ctx;
        ctx.source = &source;
        ctx.targets = targets;
        ctx.cardSource = this;

        for (auto& effect : effects) {
            std::cout << " [Effect]: " << effect->getDescription() << std::endl;
            effect->apply(ctx); 
        }
    }

    std::string getName() const { return name; }
    std::string getDescription() const { return description; } 
    bool hasEffects() const { return !effects.empty(); }
    std::string getEffectsSummary() const {
        std::string result;
        for (const auto& e : effects) {
            if (!result.empty()) result += ", ";
            result += e->getDescription();
        }
        return result;
    }

private:
    std::string name;
    std::string description;
    Suit suit;
    CardValue value;
    std::vector<std::unique_ptr<Effect>> effects;
};

