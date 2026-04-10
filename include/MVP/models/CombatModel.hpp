#pragma once

#include <memory>
#include <string>

#include <MVP/CombatBase.hpp>
#include <MVP/CombatModelListener.hpp>

struct CombatModel
{
	void ResolveSpell (ObjectID card);
	void ResolveSpell (ObjectID card, ObjectID target);
	void ResolveCapture (ObjectID card);

	bool CanCardCapture (ObjectID card);
	bool ImplicitTargets (ObjectID card);
	bool IsTargetValid (ObjectID card);
	TargetType GetSpellTarget (ObjectID card);

	bool IsExhaust (ObjectID card);

	std::string_view GetCardDescription (ObjectID card);

	std::set<Card> GetCards ();
	std::set<Enemy> GetEnemies ();

	std::unique_ptr<CombatModelListener> m_listener;
};
