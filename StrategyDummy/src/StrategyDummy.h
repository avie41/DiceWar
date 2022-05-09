#pragma once
#include "../../Commun/IStrategyLib.h"
#include "SGraph.h"
#include<vector>
#include<set>


class StrategyDummy
{
public:
	StrategyDummy(unsigned int id, unsigned int nbPlayer, const SMap* map);
	StrategyDummy(const StrategyDummy &obj) =delete;
	StrategyDummy(StrategyDummy &&obj) =delete;
	~StrategyDummy();
	StrategyDummy& operator=(const StrategyDummy &obj) =delete;
	StrategyDummy& operator=(StrategyDummy &&obj) =delete;

public:
	bool PlayTurn(unsigned int gameTurn, const SGameState *state, STurn *turn);
	inline unsigned int getId() const { return Id; }
	void Update(const SGameState* state);

protected:
	const unsigned int Id;
	const unsigned int NbPlayer;
	SMap Map;
};
