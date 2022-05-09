#pragma once

#include <vector>
#include <set>
#include <utility>
#include <map>
#include <iostream>
#include "../../Commun/IMapLib.h"

using Cell = std::pair<unsigned int, unsigned int>;
using Region = std::vector<Cell>;
using Regions = std::vector<Region>;

class MapLoader : public IMap
{
public:
	MapLoader() {}
	virtual ~MapLoader() { for (auto pMap : Maps) DeleteMap(pMap); }
	SRegions* GenerateMap(unsigned int& r, unsigned int& c) override;
	void DeleteMap(SRegions* regions) override;


private:
	SRegions* ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC);
	void generateSeedRegions(unsigned int& rows, unsigned int& cols);
	void extendRegions(const unsigned int& rows, const unsigned int& cols);
	void addEmptyRegions(unsigned int& rows, unsigned int& cols);
	Region getNeighboringCells(const Cell& currentCell, const unsigned int& rows, const unsigned int& cols, bool onlyAvailable = true);
	void deleteRegion(const Region& regionToBeDeleted);

	// Checking connexity of Map
	bool isMapConnected(const Regions& map, unsigned int const& rows, unsigned int const& cols);
	std::set<int> NeighboringCellsOfRegion(const Region& reg, const int& rows, const int& cols);


private:
	// Used for loading the map
	std::set<SRegions*> Maps;

	// Used for generating the map
	Regions Map;
	std::set<Cell> assignedCells;
};

