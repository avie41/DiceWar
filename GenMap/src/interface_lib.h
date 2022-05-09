#pragma once
// v 1.0

#include "../../Commun/IMapLib.h"
#include "../../Commun/library.h"

extern "C"
{
	// Fonction à appeler au chargement de la librairie
	// Le générateur de carte doit compléter la structure SInfo
	API_EXPORT void InitMap(SInfo* info);

	API_EXPORT IMap* MapFactory();
	API_EXPORT void MapDelete(IMap* obj);
}
