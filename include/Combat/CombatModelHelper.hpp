#pragma once

#include <set>

#include <Combat/Card.hpp>

using CardInventory = std::set<Card *>;
using ItemInventory = int; // TODO <-- when items are implemented, do the thing
using IndexedCardInventory = std::map<ObjectID, Card *>;
using IndexedItemInventory = int; // TODO <-- when items are implemented, do the thing