#pragma once

struct CombatSubscriber
{
	virtual void NotifyPlayerHover () = 0;
	virtual void NotifyPlayerPress () = 0;

	virtual void NotifyItemHover () = 0;
	virtual void NotifyItemPress () = 0;

	virtual void NotifyCardHover  () = 0;
	virtual void NotifyCardHoverStop  () = 0;
	virtual void NotifyCardPress  () = 0;
	virtual void NotifyCardDrag   () = 0;
	virtual void NotifyCardDrop   () = 0;

	virtual void NotifyEnemyHover () = 0;
	virtual void NotifyEnemyPress () = 0;
};