#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <functional>

namespace chai::graph
{
    /*
     * @brief Topologically sorts the inNodes and places the sorted indexes in outNodes
     * Returns success if no cycle is found.
     */
    template<typename T>
    inline bool topologicalSort(const std::vector<T>& inNodes, std::vector<uint32_t>& outNodes,
                         std::unordered_map<uint32_t, std::vector<uint32_t>> adjList,
                         std::vector<uint32_t> inDegree)
    {
        std::queue<uint32_t> ready;
        for (uint32_t i = 0; i < inNodes.size(); ++i)
            if (inDegree[i] == 0)
                ready.push(i);

        // An implementation of Kahn's algorithm.
        // Reference: https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm
        while (!ready.empty()) {
            uint32_t p = ready.front();
            ready.pop();
            outNodes.push_back(p);
            for (uint32_t dependent : adjList[p])
                if (--inDegree[dependent] == 0)
                    ready.push(dependent); // dependent now has no incoming edges, visit
        }

        // no cycles allowed
        if (outNodes.size() != inNodes.size()) {
            return false;
        }

        return true;
    }
}
