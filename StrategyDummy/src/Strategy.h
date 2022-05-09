#pragma once

#include "../../Commun/IStrategyLib.h"
#include "StrategyDummy.h"

class Strategy : public IStrategy
{
public:
	Strategy() : Current(nullptr) {}
	virtual ~Strategy() { delete Current; }
	void InitGame(unsigned int id, unsigned int nbPlayer, const SMap* map) override;
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn) override;
	void EndGame(unsigned int idWinner) override;

protected:
	StrategyDummy* Current;
};
