#pragma once
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/Vertex.h>
#include <glad/gl.h>
#include <vector>

namespace chai::brew
{
    class OpenGLMesh : public IMesh 
    {
    public:
        OpenGLMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& idx = {});
        ~OpenGLMesh() override;

        // Mesh interface
        void bind() const override;
        void unbind() const override;
        uint32_t getVertexCount() const override { return vertices.size(); }
        uint32_t getIndexCount() const override { return indices.size(); }
        bool isIndexed() const override { return !indices.empty(); }

        const std::vector<Vertex>& getVertices() const override { return vertices; }
        const std::vector<uint32_t>& getIndices() const override { return indices; }

        bool isUploaded() const override { return uploaded; }
        void markDirty() override { dirty = true; }

    private:
        void uploadToGPU();
        void setupVertexAttributes();
        void updateVertices(const std::vector<Vertex>& newVertices);
        void updateIndices(const std::vector<uint32_t>& newIndices);

        GLuint VAO, VBO, EBO;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        bool uploaded;
        bool dirty;
    };

    //CHAI_CLASS(OpenGLMesh, "OpenGLMesh", "1.0.0")
    //{
    //}
}