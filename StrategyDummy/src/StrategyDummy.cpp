#include "StrategyDummy.h"
#include "SGraph.h"
#include <vector>
#include <set>
#include <algorithm>
#include<iostream>


StrategyDummy::StrategyDummy(unsigned int id, unsigned int nbPlayer, const SMap* map) :
	Id(id),
	NbPlayer(nbPlayer)
{
	// faire une copie entière de la structure map localement dans l'objet Map
	Map.nbCells = map->nbCells;
	Map.cells = new SCell[Map.nbCells];

	const SCell* ref = map->cells;
	for (unsigned int i = 0; i < Map.nbCells;++i) {
		(Map.cells[i]).nbNeighbors = (ref[i]).nbNeighbors;
		(Map.cells[i]).neighbors = new SCell * [(Map.cells[i]).nbNeighbors];
		((Map.cells[i]).infos).id = ((ref[i]).infos).id;
		((Map.cells[i]).infos).nbDices = ((ref[i]).infos).nbDices;
		((Map.cells[i]).infos).owner = ((ref[i]).infos).owner;
	}

	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		for (unsigned int j = 0; j < ref[i].nbNeighbors;++j) {
			for (unsigned int k = 0; k < Map.nbCells;++k) {
				if ((((ref[i]).neighbors[j])->infos).id == (Map.cells[k]).infos.id) Map.cells[i].neighbors[j] = &Map.cells[k];
			}
		}
	}
}

StrategyDummy::~StrategyDummy()
{
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		delete[] Map.cells[i].neighbors;
	}
	delete[] Map.cells;
}

void StrategyDummy::Update(const SGameState* state) {
	// pour chaque cellule de notre Map locale
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		// on cherche l'indice correspondant à cette meme case dans l'objet pointé par state
		unsigned int j;
		for(j = 0; (state->cells)[j].id != Map.cells[i].infos.id; ++j);
		
		//puis on met à jour les informations
		Map.cells[i].infos.owner = (state->cells)[j].owner;
		Map.cells[i].infos.nbDices = (state->cells)[j].nbDices;
	}
}

bool StrategyDummy::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	Update(state);

	SGraph network(Map, Id);

	auto attack = network.GetRegionsToAttack();
	auto weak = network.FindWeakRegions();
	auto connect = network.AttackByConnectingRegions();

	for (auto c : connect) {
		auto it = std::find_if(attack.begin(), attack.end(), [&, c](const std::pair<unsigned int, unsigned int>& a) { return a.second == c; });
		if (it != attack.end() && network.test(it->first, it->second)) {
			turn->cellFrom = it->first;
			turn->cellTo = it->second;
			return true;
		}
	}

	for (auto w : weak) {
		auto it = std::find_if(attack.begin(), attack.end(), [&, w](const std::pair<unsigned int, unsigned int>& a) { return a.second == w; });
		if (it != attack.end() && network.test(it->first, it->second)) {
			turn->cellFrom = it->first;
			turn->cellTo = it->second;
			
			return true;
		}
	}

	if (attack.size() > 0) {
		for (auto a : attack) {
			if (network.test(a.first, a.second)) {
				turn->cellFrom = a.first;
				turn->cellTo = a.second;
				return true;
			}
		}
	}
	return false;	
}