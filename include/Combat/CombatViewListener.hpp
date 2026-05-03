#pragma once

struct CombatViewListener
{
	virtual void OnCardHoverStart  (ObjectID card) = 0;
	virtual void OnCardHoverStop  (ObjectID card) = 0;

	virtual void OnCardPress  (ObjectID card) = 0;

	virtual void OnCardDrag  (ObjectID card) = 0;
	virtual void OnCardDrop  (ObjectID card) = 0;

	virtual void OnEnemyHover  (ObjectID enemy) = 0;
	virtual void OnEnemyPress  (ObjectID enemy) = 0;

	virtual void DebugDrawCard () = 0;
};
