#pragma once
#include <Vertex.h>
#include <vector>

namespace chai::gfx
{
    struct MeshAsset 
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        bool empty() const { return vertices.empty() || indices.empty(); }

        bool isValid() const
        {
            if (empty() || indices.size() % 3 != 0)
                return false;
            for (uint32_t i : indices)
                if (i >= vertices.size())
                    return false;
            return true;
        }
    };
}