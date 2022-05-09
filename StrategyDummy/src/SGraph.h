#pragma once
#include "../../Commun/IStrategyLib.h"
#include<map>
#include<vector>
#include<set>
#include<memory>
#include<algorithm>

class SGraph
{
public:
	// Coplien's form
	SGraph(const SMap& map, const unsigned int& owner);
	SGraph(const SGraph& obj) = default;
	SGraph(SGraph&& obj) = default;
	~SGraph() = default;
	SGraph& operator=(const SGraph& obj) = default;
	SGraph& operator=(SGraph&& obj) = default;

	// Implementation of chosen strategies
	std::vector<std::pair<unsigned int, unsigned int>> GetRegionsToAttack();
	std::vector<unsigned int> FindWeakRegions();
	std::vector<unsigned int> AttackByConnectingRegions();

	// Utility functions
	void Update(const SMap& map);
	void ComputeAllComponents();
	bool test(unsigned int a, unsigned int b);

protected:
	// BFS Algorithm implementation
	std::set<unsigned int> BreadthFirstSearch(std::map<unsigned int, std::vector<unsigned int>>& graph,
		unsigned int start, const int& end);
	std::set<unsigned int> TracePathBack(std::map<unsigned int, unsigned int> parents,
		unsigned int start, unsigned int end);

	// Sorting function
	template<class F>
	void SortGraph(F&& f);

	// Utility function
	std::set<unsigned int> GetNeighbourRegions();

protected:
	std::map<unsigned int, SCellInfo> Vertices;
	std::map<unsigned int, std::vector<unsigned int>> Graph;
	// Links owners to their components
	std::map<unsigned int, std::vector<std::vector<unsigned int>>> Components;
	unsigned int Owner;
};


template<class F>
void SGraph::SortGraph(F && f)
{
	// Sorting the neighbours according to number of dices.
	// This allows BFS to find the shortest path between two vertices.

	for (auto& neigh : Graph) {
		std::sort(neigh.second.begin(), neigh.second.end(), [this, f](const unsigned int& a, const unsigned int& b) {
			return f(Vertices[a].nbDices, Vertices[b].nbDices);
		});
	}
}

