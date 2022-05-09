#pragma once
// v 1.0

#include "IInfoLib.h"
#include "IMapLib.h"
#include "IStrategyLib.h"

struct SGameTurn
{
	unsigned int idPlayer;
	STurn turn;
	unsigned int dices[2][8];
};

struct IComReferee
{
	virtual ~IComReferee() {}
	virtual void InitGameState(unsigned int idTurn, const SGameState* state) = 0;
	virtual void UpdateGameState(unsigned int idTurn, const SGameTurn* turn, const SGameState* finalState) = 0;
	virtual void UpdateGameStatePostTurn(unsigned int idTurn, const SGameState* finalState) = 0;
	virtual void EndGame(unsigned int idTurn, unsigned int idWinner) = 0;
};

struct IReferee
{
	virtual ~IReferee() {}
	virtual void NewGame(IComReferee* comm, IStrategy** strategies, const SRegions* map) = 0;
};

// Types prédéfinis pour simplifier la récupération des fonctions dans la librairie
using pRefereeFactory = IReferee * (*)();
using pRefereeDelete = void (*)(IReferee* obj);
