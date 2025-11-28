#include <OpenGLRenderer/GLHelpers.h>
#include <OpenGLRenderer/OpenGLMesh.h>

namespace chai
{
    brew::OpenGLMeshManager::~OpenGLMeshManager()
    {
        for (auto& [id, meshData] : m_meshCache) {
            for (auto& [shader, vao] : meshData->vaosPerShader) {
                glDeleteVertexArrays(1, &vao);
            }
            if (meshData->VBO != 0)
                glDeleteBuffers(1, &meshData->VBO);
            if (meshData->EBO != 0)
                glDeleteBuffers(1, &meshData->EBO);
        }
    }

    GLuint brew::OpenGLMeshManager::getOrCreateVAO(OpenGLMeshData* meshData,
                                                   GLuint shaderProgram,
                                                   const ShaderAsset* shaderAsset)
    {
        // Check if we already have a VAO
        if (auto it = meshData->vaosPerShader.find(shaderProgram);
            it != meshData->vaosPerShader.end()) {
            return it->second;
        }

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, meshData->VBO);
        if (meshData->EBO != 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->EBO);
        }

        // Configure vertex attributes
        for (const auto& input : shaderAsset->getVertexInputs()) {
            const VertexAttribute* meshAttr = meshData->layout.findAttribute(input.name);
            if (!meshAttr) {
                std::cout << "VAO: no mesh attribute for shader input '" << input.name
                          << "' (location " << input.location << ")\n";
                continue;
            }

            glEnableVertexAttribArray(input.location);
            glVertexAttribPointer(
                input.location,
                meshAttr->getComponentCount(),
                toGLType(meshAttr->type),
                meshAttr->normalized ? GL_TRUE : GL_FALSE,
                meshData->layout.getStride(),
                reinterpret_cast<void*>(static_cast<uintptr_t>(meshAttr->offset)));

            // Verify it was enabled
            GLint enabled = 0;
            glGetVertexAttribiv(input.location, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        }

        //glBindVertexArray(previousVAO);
        glBindVertexArray(0);

        // Cache this VAO
        meshData->vaosPerShader[shaderProgram] = vao;

        return vao;
    }
} // namespace chai