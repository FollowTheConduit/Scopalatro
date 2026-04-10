#pragma once
#include <string>
#include <vector>
#include <memory>
#include "EffectSystem.hpp"
#include "Entity.hpp"
#include "ScopaDeck.hpp"

class Card
{
public:
	Card (std::string name, std::string desc, Suit s = Suit::Denari, CardValue v = CardValue::Asso);

	
	void AddEffect (Effect && effect);
	void Play(Entity& source, std::vector<Entity*> targets);

	bool HasEffects() const;

	Suit      GetSuit () const;
	CardValue GetValue () const;
	int       GetNumericValue () const;

	std::string GetName() const;
	std::string GetDescription() const;
	std::string GetEffectsSummary() const;


private:
	std::string name;
	std::string description;

	Suit suit;
	CardValue value;

	std::vector<Effect> effects;
};

