#include "interface_lib.h"
#include "Strategy.h"
#include <iostream>
#include <fstream>

API_EXPORT void InitStrategy(SInfo* info)
{
	std::cout << "MA2S" << std::endl;

	info->SetName("MA2S");
	info->AddMember("Steven Paterour");
	info->AddMember("Alexis Vie");
	info->AddMember("Souleymane Ouattara");
	info->AddMember("Malo Maisonneuve");
}

API_EXPORT IStrategy* StrategyFactory()
{
	return(new Strategy());
}

API_EXPORT void StrategyDelete(IStrategy* obj)
{
	delete(obj);
}

// ----------------------------------------------------------------------------

void Strategy::InitGame(unsigned int id, unsigned int nbPlayer, const SMap* map)
{
	if (Current) delete Current;
	// Here we can assign various strategies for each given id!
	Current = new StrategyDummy(id, nbPlayer, map);
}

bool Strategy::PlayTurn(unsigned int gameTurn, const SGameState *state, STurn *turn)
{
	if (Current) return(Current->PlayTurn(gameTurn, state, turn));
	return(false);
}

void Strategy::EndGame(unsigned int idWinner)
{
	/* // enabled us to calculate winrate
	std::ofstream out("output.txt", std::ofstream::app);

	if (!out.fail()) {
		if (idWinner == Current->getId()) out << 1 << std::endl;
		else out << 0 << std::endl;
	}

	out.close();

	*/
	if (Current)
	{
		delete Current;
		Current = nullptr;
	}
}
