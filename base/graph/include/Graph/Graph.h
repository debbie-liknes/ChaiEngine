#pragma once

#include <vector>
#include <unordered_map>

namespace chai::graph
{
    template<typename NodeType>
    class AdjacencyList
    {
    public:
        // contains size limit even for trivially copyable types
        using StoredType = std::conditional_t<
                (std::is_trivially_copyable_v<NodeType> && sizeof(NodeType) <= 2 * sizeof(void*)),
                NodeType,
                std::reference_wrapper<NodeType>>;

        void clear()
        {
            adjList_.clear();
        }

        void add(const NodeType& u, const NodeType& v)
        {
            if constexpr (std::is_same_v<StoredType, NodeType>) {
                adjList_[u].push_back(v);
            } else {
                adjList_[std::ref(u)].push_back(std::ref(v));
            }
        }

        std::vector<StoredType> getNeighbors(const NodeType& u) const
        {
            if constexpr (std::is_same_v<StoredType, NodeType>) {
                if (auto it = adjList_.find(u); it != adjList_.end()) {
                    return it->second;
                }
            } else {
                if (auto it = adjList_.find(std::ref(u)); it != adjList_.end()) {
                    return it->second;
                }
            }
            return {};
        }

        bool operator()(const NodeType& u, const NodeType& v) const
        {
            if constexpr (std::is_same_v<StoredType, NodeType>) {
                if (auto mapIt = adjList_.find(u); mapIt != adjList_.end() &&
                    std::ranges::find(mapIt->second, v) != mapIt->second.end()) {
                    return true;
                }
            } else {
                if (auto mapIt = adjList_.find(std::ref(u));
                    mapIt != adjList_.end() &&
                    std::ranges::find(std::ref(mapIt->second), v) != mapIt->second.end()) {
                    return true;
                }
            }
            return false;
        }
    private:
        std::unordered_map<StoredType, std::vector<StoredType>> adjList_;
    };

    template<typename T>
    using NodeList = std::vector<T>;

    template<typename T>
    struct Graph {
        NodeList<T> nodes;
        AdjacencyList<uint32_t> edges;
    };
} // namespace chai::graph
