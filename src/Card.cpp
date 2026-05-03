#include <Combat/Card.hpp>

#include <string>

Card::Card(std::string name, std::string desc, Suit s, CardValue v):
	name(name), description(desc), suit(s), value(v)
{

}

//void Card::Play (Entity& source, std::vector<Entity*> targets)
//{
//	std::cout << "\n> Playing card: " << name << std::endl;
//
//	EffectContext ctx;
//	ctx.source = &source;
//	ctx.targets = targets;
//	ctx.cardSource = this;
//
//	for (auto& effect : effects)
//	{
//		std::cout << " [Effect]: " << effect->getDescription() << std::endl;
//		effect->apply (ctx); 
//	}
//}

bool Card::HasEffects () const
{
	return false;
	//return !effects.empty ();
}

Suit Card::GetSuit () const
{
	return suit;
}

CardValue Card::GetValue () const
{
	return value;
}

int Card::GetNumericValue () const
{
	return static_cast<int>(value);
}

std::string Card::GetName() const
{
	return name;
}

std::string Card::GetDescription() const
{
	return description;
}

std::string Card::GetEffectsSummary() const
{
	//std::string result;
	//for (const auto& e : effects)
	//{
	//	if (!result.empty()) result += ", ";
	//	result += e->getDescription();
	//}
	//return result;

	return "If you see this there is a bug";
}

std::string Card::GetKey () const
{
	return suitToString (suit) + "_" + valueToString (value);
}

std::string Card::GetKey (Suit s, CardValue v)
{
	return suitToString (s) + "_" + valueToString (v);
}