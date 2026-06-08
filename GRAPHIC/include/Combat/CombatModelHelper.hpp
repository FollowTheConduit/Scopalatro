#pragma once

#include <map>
#include <set>

#include <Combat/CombatHelper.hpp>
#include <Combat/Enemy.hpp>

#include <Card.hpp>


using CardInventory = std::vector<Card>;
using ItemInventory = std::vector<int>; // TODO <-- when items are implemented, do the thing

using IndexedCardTable  = std::map<ObjectID, Card *>;
using IndexedEnemyTable = std::map<ObjectID, Enemy *>;
using IndexedItemTable  = std::map<ObjectID, int>; // TODO <-- when items are implemented, do the thing