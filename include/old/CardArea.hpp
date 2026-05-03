#pragma once

#include <map>
#include <vector>

#include <renderables/CardSprite.hpp>
#include <Card.hpp>

class CardArea
{
public:
	CardArea ();

	void Move (std::vector<CardSprite> cards);
	void Move (CardArea & other);

	Card & GetCard (int index) const;

	void Reset ();

private:
	std::map<int, CardSprite> cards;
};
