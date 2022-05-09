#pragma once
// v 1.0

#include "../../Commun/IStrategyLib.h"
#include "../../Commun/library.h"

extern "C"
{
	// Fonction à appeler au chargement de la librairie
	// La stratégie doit compléter la structure SInfo
	API_EXPORT void InitStrategy(SInfo* info);

	API_EXPORT IStrategy* StrategyFactory();
	API_EXPORT void StrategyDelete(IStrategy* obj);
}
