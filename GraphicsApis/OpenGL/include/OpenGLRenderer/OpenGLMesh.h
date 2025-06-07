#pragma once
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/Vertex.h>
#include <glad/gl.h>
#include <vector>

namespace chai::brew
{
    class OpenGLMesh : public IMesh 
    {
    public:
        ~OpenGLMesh() override {
            cleanup();
        }

        void setVertices(const std::vector<Vertex>& verts) override {
            vertices = verts;
            uploaded = false;
        }

        void setIndices(const std::vector<unsigned int>& inds) override {
            indices = inds;
            uploaded = false;
        }

        void upload() override {
            if (uploaded) return;

            // Generate and bind VAO
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // Upload vertex data
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                vertices.data(), GL_STATIC_DRAW);

            // Upload index data if present
            if (!indices.empty()) {
                glGenBuffers(1, &EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                    indices.data(), GL_STATIC_DRAW);
            }

            // Set vertex attributes
            SetupVertexAttributes();

            glBindVertexArray(0);
            uploaded = true;
        }

        void draw() const override {
            if (!uploaded) return;

            glBindVertexArray(VAO);
            if (!indices.empty()) {
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            }
            glBindVertexArray(0);
        }

        size_t getVertexCount() const override { return vertices.size(); }
        size_t getTriangleCount() const override {
            return indices.empty() ? vertices.size() / 3 : indices.size() / 3;
        }
        bool isUploaded() const override { return uploaded; }

        void cleanup() override {
            if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
            if (VBO) { glDeleteBuffers(1, &VBO); VBO = 0; }
            if (EBO) { glDeleteBuffers(1, &EBO); EBO = 0; }
            uploaded = false;
        }

    private:
        void SetupVertexAttributes() {
            // Position (location 0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                (void*)offsetof(Vertex, position));
            glEnableVertexAttribArray(0);

            // Normal (location 1)
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);

            // Texture coordinates (location 2)
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                (void*)offsetof(Vertex, texCoord));
            glEnableVertexAttribArray(2);

            // Vertex color (location 3)
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(3);
        }

        GLuint VAO = 0, VBO = 0, EBO = 0;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        bool uploaded = false;

    };
}