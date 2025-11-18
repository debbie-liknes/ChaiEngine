#pragma once
#include <ChaiEngine/RenderCommandList.h>

namespace chai::brew
{
    struct RenderKey 
    {
        union 
        {
            struct 
            {
                uint64_t transparency : 1;   // 0 = opaque, 1 = transparent
                uint64_t depth : 23;  // Distance for sorting
                uint64_t shader : 16;  // Shader program for batching
                uint64_t material : 12;  // Material for batching
                uint64_t mesh : 12;  // Mesh VAO for batching
            };
            uint64_t key;
        };

        RenderKey() : key(0) {}

        bool operator<(const RenderKey& other) const 
        {
            return key < other.key;
        }
    };

    struct SortedDrawCommand 
    {
        RenderKey sortKey;
        RenderCommand command;
    };
}