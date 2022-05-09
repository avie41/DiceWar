#include "MapLoader.h"
#include "DefaultMap.h"
#include <random>
#include <time.h>
#include <algorithm>
#include <functional>

SRegions* MapLoader::GenerateMap(unsigned int& r, unsigned int& c)
{
	// initial static generation
	//Regions regions;
	//LoadDefaultMap(regions);

	// Default Size of the grid : (20,30)
	unsigned int nbR, nbC;
	generateSeedRegions(r, c);
	extendRegions(r, c);
	addEmptyRegions(r, c);
	SRegions* sregions = ConvertMap(Map, nbR, nbC);
	r = nbR;
	c = nbC;
	Maps.insert(sregions);
	return(sregions);
}

void MapLoader::DeleteMap(SRegions* regions)
{
	for (unsigned int i = 0; i < regions->nbRegions; ++i)
	{
		delete[](regions->region[i].cells);
	}
	delete[] regions->region;
	delete regions;
}

SRegions* MapLoader::ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC)
{
	SRegions* reg = new SRegions;

	nbR = 0;
	nbC = 0;
	reg->nbRegions = regions.size();
	reg->region = new SRegion[reg->nbRegions];
	for (unsigned int i = 0; i < reg->nbRegions; ++i)
	{
		reg->region[i].nbCells = regions[i].size();
		reg->region[i].cells = new SRegionCell[reg->region[i].nbCells];
		for (unsigned int j = 0; j < reg->region[i].nbCells; ++j)
		{
			if (nbR < regions[i][j].first) nbR = regions[i][j].first;
			if (nbC < regions[i][j].second) nbC = regions[i][j].second;
			reg->region[i].cells[j].y = regions[i][j].first;
			reg->region[i].cells[j].x = regions[i][j].second;
		}
	}
	return(reg);
}

/* Creates regions from spaced seed points. The regions each contain a single point. */
void MapLoader::generateSeedRegions(unsigned int& rows, unsigned int& cols) {
	// average size of a region
	const unsigned int area = 20;
	// approximation of the spacing between the centers of each territory if we assume they are square
	unsigned int l = static_cast<unsigned int>(sqrt(area));
	for (unsigned int i = l / 2; i < rows; i += l) {
		for (unsigned int j = 0; j < cols; j += l) {
			Map.push_back(Region({ Cell(i,j) }));
			assignedCells.insert(Cell(i, j));
		}
	}
}

/* To add to a region all valid neighboring cells (respecting the grid size and not already assigned) of the currentCell in parameter. */
Region MapLoader::getNeighboringCells(const Cell& currentCell, const unsigned int& rows, const unsigned int& cols, bool onlyAvailable)
{
	// The type-alias "Region" is used but here it means the list of neighboring cells.
	Region neighboringC;

	// Adding all the valid cells to the current region

	if (currentCell.first != 0) neighboringC.push_back(Cell(currentCell.first - 1, currentCell.second));
	if (currentCell.second != 0) neighboringC.push_back(Cell(currentCell.first, currentCell.second - 1));
	if (currentCell.second + 1 < cols) neighboringC.push_back(Cell(currentCell.first, currentCell.second + 1));
	if (currentCell.first + 1 < rows) neighboringC.push_back(Cell(currentCell.first + 1, currentCell.second));

	if (currentCell.first % 2 == 0) {
		if ((currentCell.first != 0) && (currentCell.second != 0)) neighboringC.push_back(Cell(currentCell.first - 1, currentCell.second - 1));
		if ((currentCell.first + 1 < rows) && (currentCell.second != 0)) neighboringC.push_back(Cell(currentCell.first + 1, currentCell.second - 1));
	}
	else {
		if ((currentCell.second + 1 < cols) && (currentCell.first + 1 < rows)) neighboringC.push_back(Cell(currentCell.first + 1, currentCell.second + 1));
		if ((currentCell.first != 0) && (currentCell.second + 1 < cols)) neighboringC.push_back(Cell(currentCell.first - 1, currentCell.second + 1));
	}
	// Deleting the cells that have already been assigned
	if (onlyAvailable)
	{
		for (auto i = neighboringC.begin(); i != neighboringC.end();) {
			if (assignedCells.find(*i) != assignedCells.end()) i = neighboringC.erase(i);
			else ++i;
		}
	}
	return neighboringC;
}
/* Expands the regions one by one */
void MapLoader::extendRegions(const unsigned int& rows, const unsigned int& cols) {
	Region neighborhood;

	std::random_device rd;
	// Mersenne Twister 19937 generator
	std::mt19937 g(rd());
	unsigned int maxNbCell = rows * cols;
	// We go through all the regions.
	// We iterate until all the cells have been allocated.
	for (auto regionIt = Map.begin(); assignedCells.size() != maxNbCell;) {
		// To randomize the order in which the cells are to be expanded.
		std::shuffle(regionIt->begin(), regionIt->end(), g);

		//We go through the cells contained in the region to expand it.
		for (auto currentCell = regionIt->begin(); currentCell != regionIt->end();) {
			// For each cell, we get its neighboring cells and add them to the current region.

			// If there are neighboring (valid) cells.
			if ((neighborhood = getNeighboringCells(*currentCell, rows, cols)).size() > 0) {
				// We go through each cell of the neighboorhood and we add it to our map
				for (auto add = neighborhood.begin(); add != neighborhood.end(); add++) {
					assignedCells.insert(*add);
					regionIt->push_back(*add);
				}
				// so that it will quit the loop and iterate on next region in order to equally expend all regions.
				currentCell = regionIt->end();
			}
			// Otherwise we go to the next cell of the region.
			else currentCell++;
		}
		regionIt++;
		// If we got to the end of the list of regions and not all the cells have been allocated yet,
		// then we start again from the beginning.
		if (regionIt == Map.end()) regionIt = Map.begin();
	}
}

// Randomly removes regions while keeping a connected component
void MapLoader::addEmptyRegions(unsigned int& rows, unsigned int& cols)
{
	// generating a seed
	std::random_device device;
	// using Mersenne Twister 19937 generator
	std::mt19937 generator(device());
	// The number of empty regions added is between 16% and 22% of the regions created.
	unsigned int min = static_cast<unsigned int>(0.16 * Map.size());
	unsigned int max = static_cast <unsigned int>(0.22 * Map.size());
	// take a random number generator and use it to generate a number within its defined target interval
	std::uniform_int_distribution<int> distribution(min, max);
	int desiredHoleRegions = distribution(generator);
	// std::cout << desiredHoleRegions << " Regions need to be deleted!" << std::endl;
	Regions emptyRegions;

	// We select a region randomly and remove it from the map.
	std::shuffle(Map.begin(), Map.end(), generator);

	bool isRegionRemoved = false;
	unsigned int i = 0;
	while (emptyRegions.size() != desiredHoleRegions)
	{
		Region  const removedRegion = Map[i];
		deleteRegion(removedRegion);

		// Region can be deleted the cells of the deleted region are removed from the assigned cells.
		if (isMapConnected(Map, rows, cols))
		{
			emptyRegions.push_back(removedRegion);
			isRegionRemoved = true;

		}
		// The map is no longer connected
		else
		{
			for (auto it = removedRegion.begin(); it != removedRegion.end(); it++)
			{
				assignedCells.insert(*it);
			}
			Map.push_back(removedRegion);
			i++;
		}

		if (i == Map.size()) {
			// If no region can be removed, we exit the while loop
			if (!isRegionRemoved) break;
		}
	}
}

// To delete a region from the map */
void MapLoader::deleteRegion(const Region& regionToBeDeleted)
{
	for (auto it = regionToBeDeleted.begin(); it != regionToBeDeleted.end(); it++)
	{
		assignedCells.erase(*it);
	}

	Map.erase(std::remove(Map.begin(), Map.end(), regionToBeDeleted), Map.end());
}

int CellToId(const Cell& cell, const int& cols) {
	return cell.first * cols + cell.second;
}

std::set<int> MapLoader::NeighboringCellsOfRegion(const Region& reg, const int& rows, const int& cols) {
	std::set<int> output;

	for (const auto& cell : reg) {
		// For each cell of the region, we get the neighboring cells
		for (const Cell& neigh : getNeighboringCells(cell, rows, cols, false)) {
			// For each neighbor of the cell
			// We add it to the output only if it is not in the current region
			if (std::find(reg.begin(), reg.end(), neigh) == reg.end()) {
				// Add the cell id to the output
				output.insert(CellToId(neigh, cols));
			}
		}
	}
	return output;
}

// Checking that Map forms a connected component
bool MapLoader::isMapConnected(const Regions& map, unsigned int const& rows, unsigned int const& cols)
{
	// 1. Mapping cell id to region id
	std::map<int, int> parent;

	for (unsigned int i = 0; i < map.size(); i++) {
		for (unsigned int j = 0; j < map[i].size(); j++) {
			parent[CellToId(map[i][j], cols)] = i;
		}
	}

	// 2. Creating the neighborhood graph
	std::set<std::pair<int, int>> graph;

	for (unsigned int i = 0; i < map.size(); i++) {
		for (const int& it : NeighboringCellsOfRegion(map[i], rows, cols)) {
			std::pair<int, int> edge(i, parent[it]);
			graph.insert(edge);
		}
	}

	// 3. Use the graph to check connectivity
	/*
		_Principle_ : Starting from the first region, we try and reach all the others.
		This mecanism is built around two variables : opened & closed.
		At first, a region is opened. Then, its neighboring regions become opened while
		the previously opened region becomes closed.
		When there isn't any more opened cells, we see if closed contains all the regions.
		If not, then the graph isn't well connected.
	*/

	std::set<int> closed{};
	std::set<int> opened{};

	// We start at region 0
	opened.insert(0);

	while (opened.size() > 0) {
		std::set<int> previously_opened = opened;
		std::set<int> to_be_opened{};

		for (auto regionId : opened) {
			// Finding all pairs that begin with regionId
			for (const auto& p : graph) {
				// Also checking that destination isn't already closed
				if (p.first == regionId && closed.find(p.second) == closed.end() && opened.find(p.second) == opened.end()) {
					to_be_opened.insert(p.second);
				}
			}
		}

		closed.insert(opened.begin(), opened.end());
		opened = to_be_opened;
	}
	return closed.size() == map.size();
}

