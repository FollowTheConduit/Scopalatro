
#include <Combat/HandArea.hpp>

#include <Core/Logger.hpp>

#include <algorithm>

using namespace TLOT;

void HandArea::AddCard (ObjectID card)
{
	if (m_hand.find (card) == m_hand.end ())
	{
		m_hand.emplace (card, m_nextIndex++);
	}
}

void HandArea::RemoveCard (ObjectID card)
{
	if (m_hand.find (card) == m_hand.end ())
	{
		return;
	}

	m_hand.erase (card);
	RecalculateIndices ();
	
	if (m_nextIndex != 0) m_nextIndex--;
}

size_t HandArea::GetHandSize ()
{
	return m_hand.size ();
}

glm::vec3 HandArea::GetCardPos (ObjectID card)
{
	if (m_hand.find (card) == m_hand.end ()) return glm::vec3 {-1000.0f};

	size_t index = m_hand[card];

	float handSize = (float)GetHandSize ();

	if (m_draggedCard != InvalidObject)
	{
		auto draggedCardIndex = m_hand.at (m_draggedCard);
		handSize -= 1.0f;
	
		if (index < draggedCardIndex)
		{
			//index += 1;
		}
		else if (index > draggedCardIndex)
		{
			index -= 1;
		}
		else
		{
			return {0.0, 0.0, -1000.0};
		}
	}

	float transformedIndex = (float)index - (handSize / 2.0f);

	float paddingX = -handSize * handSize + 95.0f;

	float x = beginX + (width / 2) + (paddingX / 2) + transformedIndex * (cardSize + paddingX) + cardSize / 2;
	float y = cardSize / 3;
	float z = index * 0.1f;

	if (m_hoveredCard != InvalidObject && m_draggedCard == InvalidObject)
	{
		auto hoveredCardIndex = m_hand.at (m_hoveredCard);
	
		float offsetX = std::abs (cardSize / (2.0f * hoverFactor) - paddingX) * hoverFactor;

		if (index < hoveredCardIndex)
		{
			x -= offsetX; // TODO: correct the logic here
		}
		else if (index > hoveredCardIndex)
		{
			x += offsetX + cardSize / 2;
		}
		else
		{
			y = cardSize + (GetCardSize (card).y / 2.0);
			x += GetCardSize (card).x / (4.0f * hoverFactor);
		}
	}


	return glm::vec3 {x, y, z};
}

glm::vec3 HandArea::GetCardSize (ObjectID card)
{
	if (m_hand.find (card) == m_hand.end ()) return glm::vec3 {-1000.0f};

	if (card == m_hoveredCard || card == m_draggedCard)
	{
		return glm::vec3 {cardSize} * hoverFactor;
	}

	return glm::vec3 {cardSize};
}

size_t HandArea::GetIndex (ObjectID card)
{
	if (m_hand.find (card) == m_hand.end ()) return -1;

	return m_hand[card];
}

void HandArea::SetHover (ObjectID card)
{
	m_hoveredCard = card;
}

void HandArea::SetDrag (ObjectID card)
{
	m_draggedCard = card;
}

void HandArea::RecalculateIndices ()
{
	std::vector<std::pair<ObjectID, size_t>> sortedHand;
	sortedHand.reserve (m_hand.size ());

	for (auto & p : m_hand)
	{
		sortedHand.push_back (p);
	}

	std::sort (sortedHand.begin (), sortedHand.end (), [] (std::pair<ObjectID, size_t> & a, std::pair<ObjectID, size_t> & b) {
		return a.second < b.second;
	});

	m_hand.clear ();

	size_t index = 0;
	for (auto & [ID, _] : sortedHand)
	{
		m_hand[ID] = index++;
	}
}
