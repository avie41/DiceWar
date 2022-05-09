#pragma once
// v 1.0

#include "IInfoLib.h"
#include "IRefereeLib.h"

struct IGui
{
	virtual ~IGui() {}
	virtual void InitGui(unsigned int nbPlayer, const SRegions *map) = 0;
	virtual void SetPlayerInfo(unsigned int id, const SInfo *info) = 0;
	virtual void WaitReady() = 0;
	virtual void PlayerError(unsigned int idPlayer) = 0;
	virtual void SetGameState(unsigned int idTurn, const SGameState* state) = 0;
	virtual void UpdateGameState(unsigned int idTurn, const SGameTurn* turn, const SGameState* finalState) = 0;
	virtual void EndGame(unsigned int idTurn, unsigned int idWinner) = 0;
};

// Types prédéfinis pour simplifier la récupération des fonctions dans la librairie
using pGuiFactory = IGui* (*)();
using pGuiDelete = void (*)(IGui *obj);
