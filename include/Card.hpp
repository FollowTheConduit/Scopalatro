#pragma once
#include <string>
#include <vector>
#include <memory>

#include <type_traits>

#include <string>

enum class Suit
{
    Coppe,   // Coupes
    Denari,  // Deniers
    Spada,   // Epées
    Bastoni, // Bâtons
};

enum class CardValue
{
    Asso = 1,
    Due = 2,
    Tre = 3,
    Quattro = 4,
    Cinque = 5,
    Sei = 6,
    Sette = 7,
    Fante = 8,   // Valet
    Cavallo = 9, // Cavalier
    Re = 10,     // Roi
};

inline std::string suitToString(Suit s) {
    switch(s) {
        case Suit::Coppe: return "cup";
        case Suit::Denari: return "deny";
        case Suit::Spada: return "sword";
        case Suit::Bastoni: return "club";
        default: return "unknown";
    }
}

inline std::string valueToString(CardValue v) {
    switch(v) {
        case CardValue::Fante: return "fante";
        case CardValue::Cavallo: return "knight";
        case CardValue::Re: return "king";
        case CardValue::Asso: return "ace";
        default: return std::to_string(static_cast<int>(v));
    }
}

class Card
{
public:
	Card (std::string name, std::string desc, Suit s = Suit::Denari, CardValue v = CardValue::Asso);

	//template<class EffectType, class ... Args>
	//void AddEffect (Args ... args)
	//{
	//	static_assert(std::is_base_of_v<Effect, EffectType>, "Class must inherit Effect");
	//
	//	effects.push_back (std::make_unique<EffectType> (args...));
	//}

	//void Play(Entity& source, std::vector<Entity*> targets);

	bool HasEffects() const;

	Suit      GetSuit () const;
	CardValue GetValue () const;
	int       GetNumericValue () const;

	std::string GetName() const;
	std::string GetDescription() const;
	std::string GetEffectsSummary() const;

	std::string GetKey () const;
	static std::string GetKey (Suit s, CardValue v);
	
private:
	std::string name;
	std::string description;

	Suit suit;
	CardValue value;

	//std::vector<std::unique_ptr<Effect>> effects;
};

