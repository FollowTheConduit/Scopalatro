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

class TableArea
{
public:
	float cardSize = 200;
	float cardAdvance = 150;

	float beginX = 0;
	float beginY = 0;

	void AddCard(ObjectID card);
	void RemoveCard(ObjectID card);

	glm::vec3 GetPos(ObjectID card);
	glm::vec3 GetScale(ObjectID card);

	auto begin() { return m_table.begin(); }
	auto end() { return m_table.end(); }

private:
	std::map<ObjectID, size_t> m_table;

	size_t m_nextIndex = 0;
};
