#pragma once
#include <glad/gl.h>
#include <ChaiEngine/UniformBuffer.h>
#include <vector>

namespace chai::brew
{
    struct OpenGLUniformBuffer
    {
        GLuint ubo = 0;
        size_t size = 0;
        std::vector<uint8_t> data;
        bool dirty = true;
    };

    class UniformManager
    {
    public:
        UniformManager() = default;

        ~UniformManager()
        {
            for (auto& [id, uniformBuffer] : m_uniformBuffers)
            {
                if (uniformBuffer->ubo != 0)
                {
                    glDeleteBuffers(1, &uniformBuffer->ubo);
                }
            }
        }

        UniformManager(const UniformManager&) = delete;
        UniformManager& operator=(const UniformManager&) = delete;

        UniformManager(UniformManager&&) = default;
        UniformManager& operator=(UniformManager&&) = default;

        void buildUniforms(std::vector<UniformBufferBase*> const& uniforms)
        {
            for (auto& u : uniforms)
            {
                auto openglBuff = std::make_unique<OpenGLUniformBuffer>();
                openglBuff->size = u->getSize();
                openglBuff->data.resize(openglBuff->size);
                u->getData(openglBuff->data.data(), u->getSize());

                glGenBuffers(1, &openglBuff->ubo);
                glBindBuffer(GL_UNIFORM_BUFFER, openglBuff->ubo);
                glBufferData(GL_UNIFORM_BUFFER, openglBuff->size, openglBuff->data.data(), GL_DYNAMIC_DRAW);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);

                m_uniformBuffers[u->getId()] = std::move(openglBuff);
            }
        }

        void updateUniform(const UniformBufferBase& uniform)
        {
            auto it = m_uniformBuffers.find(uniform.getId());
            assert(it != m_uniformBuffers.end());
            auto& uniformBuffer = it->second;

            uniform.getData(uniformBuffer->data.data(), uniform.getSize());

            glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer->ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBuffer->size, uniformBuffer->data.data());

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        OpenGLUniformBuffer* getUniformBufferData(const UniformBufferBase& uniform) const
        {
            auto it = m_uniformBuffers.find(uniform.getId());
            if (it != m_uniformBuffers.end())
            {
                return it->second.get();
            }
            return nullptr;
        }

    private:
        CMap<GLuint, std::unique_ptr<OpenGLUniformBuffer>> m_uniformBuffers;
    };
}