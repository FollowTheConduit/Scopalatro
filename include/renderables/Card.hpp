#pragma once

#include <map>

#include <modules/Renderable.hpp>

class CardColor
{
public:
	enum Color
	{
		Club,
		Deny,
		Cup,
		Sword
	};

	CardColor () = default;
	CardColor (Color value) : mColor {value} {}

	constexpr bool operator== (Color value) {return value == mColor;}
	constexpr bool operator!= (Color value) {return value != mColor;}

	std::string const ToString () const {
		return ToString (mColor);
	}

	static std::string const ToString (Color value)
	{
		static std::map<Color, std::string> const dictionary = {
			{Club, "club"},
			{Deny, "deny"},
			{Cup, "cup"},
			{Sword, "sword"},
		};

		return dictionary.at (value);
	}

private:
	Color mColor;
};

class CardSymbol {
public:
	enum Symbol
	{
		Ace, Two, Three, Four, Five, Six, Seven, Jack, Queen, King
	};

	CardSymbol () = default;
	CardSymbol (Symbol value) : mSymbol {value} {}
	CardSymbol (int value)
	{
		static std::map<int, Symbol> const dictionary = {
			{1, Ace   },
			{2, Two   },
			{3, Three },
			{4, Four  },
			{5, Five  },
			{6, Six   },
			{7, Seven },
			{8, Jack  },
			{9, Queen },
			{10, King }
		};

		mSymbol = dictionary.at (value);
	}

	constexpr bool operator== (Symbol value) { return value == mSymbol; }
	constexpr bool operator!= (Symbol value) { return value != mSymbol; }

	std::string const ToString () const
	{
		return ToString (mSymbol);
	}

	static std::string const ToString (Symbol value)
	{
		static std::map<Symbol, std::string> const dictionary = {
			{Ace,   "ace"},
			{Two,   "two"},
			{Three, "three"},
			{Four,  "four"},
			{Five,  "five"},
			{Six,   "six"},
			{Seven, "seven"},
			{Jack,  "jack"},
			{Queen, "queen"},
			{King,  "king"}
		};

		return dictionary.at (value);
	}

private:
	Symbol mSymbol;
};

class Card
{
public:
	// maybe create a kind of cache and return a string_view instead ?
	static std::string const GetKey (CardColor color, CardSymbol symbol)
	{
		return color.ToString () + "_" + symbol.ToString ();
	}

private:
	Card (CardColor color, CardSymbol symbol, TLOT::Renderable renderable):
		m_color {color}, m_symbol {symbol}
	{

	}

	CardColor  m_color;
	CardSymbol m_symbol;

	TLOT::Renderable m_renderable;

	friend class CardManager;
};

class CardManager
{
public:
	CardManager (TLOT::RenderableManager & renderableManager);
	Card Create (CardColor color, CardSymbol symbol);

private:
	TLOT::RenderableManager & m_renderableManager;


	TLOT::ResourceHandle m_quadMesh;
	std::map<std::string, TLOT::ResourceHandle> m_textures;
	TLOT::ResourceHandle m_backNormal;
};

