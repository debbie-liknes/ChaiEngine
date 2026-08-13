#pragma once

#include <queue>
#include <cstdint>

#include <Graph/Graph.h>

namespace chai::graph
{
    /*
     * @brief Topologically sorts the inGraph and places the sorted indexes in outNodes
     * Returns success if no cycle is found.
     */
    template<typename T>
    inline bool topologicalSort(const Graph<T>& inGraph, NodeList<uint32_t>& outNodes)
    {
        std::vector<uint32_t> inDegree;
        inDegree.resize(inGraph.nodes.size());

        for (uint32_t u = 0; u < inGraph.nodes.size(); ++u)
            for (auto& v : inGraph.edges.getNeighbors(u))
                inDegree[v]++;

        std::queue<uint32_t> ready;
        for (uint32_t i = 0; i < inGraph.nodes.size(); ++i)
            if (inDegree[i] == 0)
                ready.push(i);

        // An implementation of Kahn's algorithm.
        // Reference: https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm
        while (!ready.empty()) {
            uint32_t u = ready.front();
            ready.pop();
            outNodes.push_back(u);
            for (uint32_t v : inGraph.edges.getNeighbors(u))
                if (--inDegree[v] == 0)
                    ready.push(v); // dependent now has no incoming edges, visit
        }

        // no cycles allowed
        if (outNodes.size() != inGraph.nodes.size()) {
            return false;
        }

        return true;
    }
}
