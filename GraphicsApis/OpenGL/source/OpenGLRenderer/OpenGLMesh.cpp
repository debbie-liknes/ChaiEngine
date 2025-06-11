#include <OpenGLRenderer/OpenGLMesh.h>
#include <iostream>

namespace chai::brew
{
	OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& idx)
		: vertices(verts), indices(idx), uploaded(false), dirty(true), VAO(0), VBO(0), EBO(0)
	{

        // Generate OpenGL objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        if (!indices.empty()) {
            glGenBuffers(1, &EBO);
        }

        // Upload data immediately
        uploadToGPU();
	}

    OpenGLMesh::~OpenGLMesh() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }

    void OpenGLMesh::bind() const {
        if (dirty && uploaded) {
            // Re-upload if data changed
            const_cast<OpenGLMesh*>(this)->uploadToGPU();
        }

        glBindVertexArray(VAO);
    }

    void OpenGLMesh::unbind() const {
        glBindVertexArray(0);
    }

    void OpenGLMesh::uploadToGPU() {
        if (vertices.empty()) {
            std::cerr << "Warning: Attempting to upload empty mesh data\n";
            return;
        }

        glBindVertexArray(VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW);

        // Upload index data if present
        if (!indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(uint32_t),
                indices.data(),
                GL_STATIC_DRAW);
        }

        // Setup vertex attributes
        setupVertexAttributes();

        // Unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (EBO) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        uploaded = true;
        dirty = false;

        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error during mesh upload: " << error << std::endl;
        }
    }

    void OpenGLMesh::setupVertexAttributes() {
        // Position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        // Normal attribute (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute (location = 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);
    }

    // Additional utility methods
    void OpenGLMesh::updateVertices(const std::vector<Vertex>& newVertices) {
        vertices = newVertices;
        markDirty();
    }

    void OpenGLMesh::updateIndices(const std::vector<uint32_t>& newIndices) {
        indices = newIndices;

        // If we didn't have an EBO before but need one now
        if (EBO == 0 && !indices.empty()) {
            glGenBuffers(1, &EBO);
        }

        markDirty();
    }
}