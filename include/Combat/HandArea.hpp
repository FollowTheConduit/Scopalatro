#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <Combat/CombatHelper.hpp>

#include <glm/glm.hpp>

class HandArea
{
public:
	float cardSize;
	float width;
	float beginX;

	void AddCard (ObjectID card);

	void RemoveCard (ObjectID card);

	size_t GetHandSize ();
	glm::vec3 GetCardPos (ObjectID card);

	glm::vec3 GetCardSize (ObjectID card);

	size_t GetIndex (ObjectID card);

	void SetHover (ObjectID card);
	void SetDrag (ObjectID card);
	auto begin () {return m_hand.begin (); }
	auto end   () {return m_hand.end   (); }

private:
	void RecalculateIndices ();

	std::map<ObjectID, size_t> m_hand;

	ObjectID m_hoveredCard = (ObjectID)-1;
	ObjectID m_draggedCard = (ObjectID)-1;

	size_t m_nextIndex = 0;
};
