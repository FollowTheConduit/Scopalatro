#pragma once

#include <map>
#include <set>

#include <Combat/Card.hpp>
#include <Combat/CombatHelper.hpp>
#include <Combat/Enemy.hpp>


using CardInventory = std::set<Card *>;
using ItemInventory = int; // TODO <-- when items are implemented, do the thing

using IndexedCardTable  = std::map<ObjectID, Card *>;
using IndexedEnemyTable = std::map<ObjectID, Enemy *>;
using IndexedItemTable  = int; // TODO <-- when items are implemented, do the thing