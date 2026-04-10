#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

#include <modules/Renderable.hpp>

using ObjectID = uint64_t; // <--- an entity (player or enemy) should have a unique ID, this is this ID
using DisplayEventID = uint64_t; // <--- the id of a display event

enum class TargetType
{
	Self,
	Enemy,
	AllEnemy,
	All
};

class EntityActor
{
public:
	bool RayIntersect2D (double x, double y, glm::vec3 lookAt) const
	{
		// determine if a ray comming from the point (x, y) directed by the camera is intersecting with the actor
		// could be in a physics module of the engine instead of this random game class

		return true;
	}
};

class Card
{

};

class CardActor: public EntityActor {};

class Enemy
{

};

class EnemyActor: public EntityActor {};

struct DisplayEvent
{
	double ms;
	TLOT::Renderable renderable;

	void Kill ()
	{
		renderable.Kill ();
	}
};
