#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>
#include <set>


#include <modules/AssetManager.hpp>

#include <opengl/RenderContext.hpp>


#include <CardActor.hpp>
#include <Card.hpp>

#include <Enemy.hpp>

enum class Turn
{
	Idle,
	Enemy,
	Player
};

enum class PlayingPhase
{
	InitialDraw,
	PlayingCards,
	TurnEnd
};

using ActionID = uint32_t;
using EnemyID  = uint32_t;

class Scopalatro
{
public:
	Scopalatro ();

	struct RunParameters
	{

	};

	bool GenerateNewRun (RunParameters const & params);
	bool LoadSave (std::filesystem::path url);
	void ResetRun ();

	struct CombatDescriptor
	{
		std::vector<EnemyID> m_enemies;
	};

	void EnterCombat (CombatDescriptor const & descriptor);

private:
	// Engine Stuff
	
	TLOT::AssetManager m_assetManager;
	TLOT::RenderContext m_renderContext;

	// Tables

	struct EnemyDescriptor
	{
		std::string name;

		int maxLife;

		std::set<ActionID> m_actionTable;
	};

	std::map<EnemyID, EnemyDescriptor> m_enemyTable;
	std::map<ActionID, int> m_enemyActionTable;


	// Player Stats

	int m_maxLife;
	int m_currentLife;
	int m_gold;

	std::set<Card> m_playerCards;
	// TODO : implement relics & co...


	// Combat

	std::vector<std::unique_ptr<Enemy>> m_enemies;
};
