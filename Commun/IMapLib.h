#pragma once
// v 1.0

#include <cstddef>
#include "IInfoLib.h"

// Structure définissant une cellule hexagonale
struct SRegionCell
{
	unsigned int x;
	unsigned int y;
};

// Structure définissant une région : ensemble de cellules hexagonales
struct SRegion
{
	SRegionCell *cells;
	size_t nbCells;
};

// Ensemble de régions
struct SRegions
{
	SRegion *region;
	size_t nbRegions;
};

struct IMap
{
	virtual ~IMap() {}

	// Fonction à appeler pour générer une nouvelle carte
	//	les variables r et c indiquent la taille de la carte à générer
	// ces valeurs peuvent être modifiées par le générateur de carte
	virtual SRegions* GenerateMap(unsigned int &r, unsigned int &c) = 0;

	// Fonction à appeler pour détruire les structures allouées lors de la génération de carte.
	virtual void DeleteMap(SRegions *regions) = 0;
};

// Types prédéfinis pour simplifier la récupération des fonctions dans la librairie
using pMapFactory = IMap * (*)();
using pMapDelete = void (*)(IMap* obj);
