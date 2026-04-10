#pragma once

class CombatModelListener
{
	virtual void NotifyCardsDrawn     () = 0;
	virtual void NotifyCardsExhausted () = 0;

	virtual void NotifyPVChange () = 0;
	virtual void NotifyDeath    () = 0;

	virtual void NotifyEndCombat () = 0;
};
