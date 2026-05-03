#include <Scopalatro.hpp>


namespace fs = std::filesystem;

Scopalatro::Scopalatro ()
{
	// TODO: check max res and init with that in mind
	m_renderContext.Init (1600, 900, "SCOPALATRO");
}

bool Scopalatro::GenerateNewRun (RunParameters const & param)
{

	// for now a player always begin with the same cards
	for (auto const & suit : {Suit::Bastoni, Suit::Coppe, Suit::Denari, Suit::Spada})
	{
		for (auto const & value : { 
			CardValue::Asso,    CardValue::Due,     CardValue::Tre,
			CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
			CardValue::Sette,   CardValue::Cavallo, CardValue::Fante,
			CardValue::Re
		})
		{
			//m_playerCards.emplace ("Normal Playing Card", "Placeholder Description", suit, value);
		}
	}

	return true;
}

bool Scopalatro::LoadSave (fs::path url)
{
	return false;
}

void Scopalatro::EnterCombat (CombatDescriptor const & descriptor)
{
	
}

void Scopalatro::ResetRun ()
{
	m_playerCards.clear ();
}