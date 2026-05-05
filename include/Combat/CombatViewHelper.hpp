#pragma once

#include <stdint.h>
#include <vector>

#include <modules/Renderable.hpp>
#include <modules/Sprite.hpp>

#include <Combat/CombatHelper.hpp>

using IndexedActorsTable  = std::map<ObjectID, TLOT::Renderable>;
using IndexedSpritesTable = std::map<ObjectID, TLOT::Sprite>;