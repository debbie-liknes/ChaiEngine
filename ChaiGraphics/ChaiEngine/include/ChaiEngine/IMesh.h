#pragma once
#include <ChaiGraphicsExport.h>
#include <vector>

namespace chai::brew
{
    struct Vertex;
    struct Color;

    class IMesh {
    public:
        virtual ~IMesh() = default;

        // Core functionality
        virtual void setVertices(const std::vector<Vertex>& vertices) = 0;
        virtual void setIndices(const std::vector<unsigned int>& indices) = 0;
        virtual void upload() = 0;
        virtual void draw() const = 0;

        // Query methods
        virtual size_t getVertexCount() const = 0;
        virtual size_t getTriangleCount() const = 0;
        virtual bool isUploaded() const = 0;

        // Resource management
        virtual void cleanup() = 0;
    };
}