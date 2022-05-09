#include "SGraph.h"
#include<set>
#include<queue>
#include<cmath>
#include<iostream>

SGraph::SGraph(const SMap& map, const unsigned int& owner) : Owner(owner)
{
	// Filling Graph & Vertices variables
	for (unsigned int i = 0; i < map.nbCells; i++) {
		// Adding the cell to Vertices
		Vertices[map.cells[i].infos.id] = map.cells[i].infos;

		// Grabbing the neighbors
		std::vector<unsigned int> neighbours;
		for (unsigned int j = 0; j < map.cells[i].nbNeighbors; j++) {
			neighbours.push_back(map.cells[i].neighbors[j]->infos.id);
		}

		// Mapping the cell id to its neighbours id
		Graph[map.cells[i].infos.id] = neighbours;
	}

	// By default, sorting Graph neighbours in ascending order
	SortGraph([](unsigned int a, unsigned int b) { return a < b; });

	ComputeAllComponents();
}

void SGraph::ComputeAllComponents()
{
	// Lists each player's components
	std::set<unsigned int> closed;

	for (const auto &v : Vertices) {
		if (closed.find(v.first) == closed.end()) {
			auto search = BreadthFirstSearch(Graph, v.first, -1);
			if (search.size() == 0) search = { v.first };

			std::vector<unsigned int> search_vec(search.begin(), search.end());

			Components[v.second.owner].push_back(search_vec);
			closed.insert(search.begin(), search.end());
		}
	}
}

bool SGraph::test(unsigned int a, unsigned int b)
{
	return (Vertices[a].nbDices >= Vertices[b].nbDices && Vertices[a].nbDices > 1 && Vertices[a].owner != Vertices[b].owner && Vertices[a].owner == Owner);
}

std::vector<unsigned int> SGraph::AttackByConnectingRegions()
{
	std::vector<unsigned int> output{};
	auto regions = Components[Owner];

	if (regions.size() <= 1) return output;

	/*
	std::cout << regions.size() << " ";
	for (auto i : regions) std::cout << i.size() << " ";
	std::cout << std::endl;
	*/

	for (unsigned int i = 0; i < regions.size(); i++) {
		
		for (unsigned int j = i+1; j < regions.size(); j++) {
			
			// For each combination of two components
			unsigned int minPathDist = 999999999;
			std::vector<unsigned int> minPath{};

			for (unsigned int k = 0; k < regions[i].size(); k++) {
				
				for (unsigned int l = 0; l < regions[j].size(); l++) {
					// For each combination of two regions
					auto search = BreadthFirstSearch(Graph, regions[i][k], regions[j][l]);

					std::vector<unsigned int> search_vec(search.begin(), search.end());

					if (search.size() == minPathDist) {
						minPath.insert(minPath.end(), search_vec.begin(), search_vec.end());
					}
					else if (search.size() < minPathDist) {
						minPath.clear();
						minPath = search_vec;
						minPathDist = search.size();
					}
				}
			}

			output.insert(output.end(), minPath.begin(), minPath.end());
		}
	}
	return output;
}

void SGraph::Update(const SMap& map)
{
	// Updating the information on each cell
	for (unsigned int i = 0; i < Vertices.size(); i++) {
		// Adding the cell to Vertices
		Vertices[map.cells[i].infos.id] = map.cells[i].infos;
	}

	// By default, sorting Graph neighbours in ascending order
	SortGraph([](unsigned int a, unsigned int b) { return a < b; });

	ComputeAllComponents();
}

std::vector<std::pair<unsigned int, unsigned int>> SGraph::GetRegionsToAttack()
{
	std::vector<std::pair<unsigned int, unsigned int>> output;

	// Get neighboring cells
	auto neighbours = GetNeighbourRegions();

	// i is used to attack cells with equal number of dices when we can't attack anywhere else
	unsigned int i = 0;

	// Keep the ones where we have a chance to attack, 
	// and also keeping track of where we attack from
	while (output.size() == 0 && i < 2) {
		for (auto neigh : neighbours) {
			for (auto region : Graph[neigh]) {
				if (Vertices[region].owner == Owner && Vertices[region].nbDices + i > Vertices[neigh].nbDices && 
					Vertices[region].nbDices > 1) {
					output.push_back(std::pair(region, neigh));
				}
			}
		}

		if (output.size() == 0) {
			// If no move can be made with strict equality, we attack
			// regions with same number of dices
			i++;
		}
		else {
			// If we already have some attackable regions, we stop there
			break;
		}
	}

	// Sort them decreasingly by our number of dices 
	std::sort(output.begin(), output.end(), [this](const std::pair<unsigned int, unsigned int>& a,
		const std::pair<unsigned int, unsigned int>& b) {
			if (Vertices[a.second].nbDices != Vertices[b.second].nbDices) {
				return Vertices[a.second].nbDices > Vertices[b.second].nbDices;
			}
			else {
				// If enemy cells have equal number of dices, we favor biggest margin between both regions
				return (int(Vertices[a.first].nbDices - Vertices[a.second].nbDices) > int(Vertices[b.first].nbDices - Vertices[b.second].nbDices));
			}
			
		});

	if ((!empty(output)) && (Vertices[output[0].second].nbDices == Vertices[output[0].first].nbDices) && (Vertices[output[0].first].nbDices != 8))
		// we accept equality only if its 8 8 equality to not take risks too early 
		output.clear();
	
	return output;
}

std::set<unsigned int> SGraph::GetNeighbourRegions()
{
	std::set<unsigned int> output;

	// Running through cells belonging to owner
	for (auto regions : Components[Owner]) {
		for (auto cell : regions) {
			// Adding the neighbours as cells to attack
			for (auto neigh : Graph[cell]) {
				if (Vertices[neigh].owner != Owner) {
					output.insert(neigh);
				}
			}
		}
	}

	return output;
}

std::set<unsigned int> SGraph::BreadthFirstSearch(std::map<unsigned int, std::vector<unsigned int>>& graph, 
	unsigned int start, const int& end)
{
	// Returns every cell encountered in search starting at the given id
	// If end is not equal to -1, then a path to that cell is returned
	std::set<unsigned int> discovered;
	std::queue<unsigned int> q;
	std::map<unsigned int, unsigned int> parents;

	unsigned int current_owner = Vertices[start].owner;
	q.push(start);

	// Running through the neighbours until there's none left.
	// We limit the search to  the owner of the given cell.
	while (q.size() > 0) {
		unsigned int vertex = q.front();
		q.pop();

		//if (q.size() == 0) discovered.insert(vertex);

		if (vertex == end) return TracePathBack(parents, start, end);

		if (graph.find(vertex) != graph.end()) {
			// If the vertex exists in the graph
			for (auto neigh : graph[vertex]) {
				// Following conditions allows for 2 uses of this function:
				// --> If end < 0, then we go through the whole component around the start cell
				// --> Else, we stop as soon as we found the end cell, and no restrictions on cell owners

				if ((discovered.find(neigh) == discovered.end()) && (!(end < 0 && current_owner == Vertices[neigh].owner) != !(end >= 0))) {
					discovered.insert(neigh);
					q.push(neigh);

					parents[neigh] = vertex;
				}
			}
		}
	}
	//std::cout << "BFS is done." << std::endl;
	return discovered; 
}

std::set<unsigned int> SGraph::TracePathBack(std::map<unsigned int, unsigned int> parents, unsigned int start, unsigned int end)
{
	// WARNING: This functions does not return a sorted path. It returns the 
	// cells composing the path in an unorderly fashion.

	std::set<unsigned int> output;
	unsigned int current = end;

	while (current != start) {
		output.insert(current);
		current = parents[current];
	}

	return output;
}

std::vector<unsigned int> SGraph::FindWeakRegions()
{
	std::vector<unsigned int> output;
	std::vector<std::pair<unsigned int, float>> output_score;

	for (auto it : Components) {
		if (it.first != Owner) { 
			// composante connexe de l'ennemi

			for (auto comp : it.second) {
				for (auto region : comp) {
					//parcourt d'une composante connexe

					// composantes connexe avant la suppression
					auto composante1 = BreadthFirstSearch(Graph, region, -1); 
					
					if (composante1.size() > 2) {
						composante1.erase(region);
						// Removing a territory
						auto copy = Graph;
						copy.erase(region);

						// composantes connexe après la suppression
						auto itr = composante1.begin();
						auto composante2 = BreadthFirstSearch(copy, *itr, -1);

						//return l'id de la cellule critique si on a plus de composantes après la suppression d'une cellule
						if (composante1.size() > composante2.size()) {
							// Score is computed in order to favor a 50/50 splitting rather than 1/3 for example
							float score = std::abs((float)composante1.size() / (composante1.size() + composante2.size()) - 0.5);
							output_score.push_back(std::pair(region, score));
						}
					}
				}
			}
		}
	}

	std::sort(output_score.begin(), output_score.end(), [this](std::pair<unsigned int, unsigned int> a, std::pair<unsigned int, unsigned int> b) {
		return a.second < b.second;
		});

	std::transform(output_score.begin(), output_score.end(), std::back_inserter(output), [](const std::pair<unsigned int, float>& elt) {return elt.first; });

	return output;
}
