#pragma once

#include <stdint.h>
#include <vector>

#include <Renderer/Renderables/Model.hpp>
#include <Renderer/Renderables/Sprite.hpp>

#include <Combat/CombatHelper.hpp>

using IndexedActorsTable  = std::map<ObjectID, TLOT::Model>;
using IndexedSpritesTable = std::map<ObjectID, TLOT::Sprite>;