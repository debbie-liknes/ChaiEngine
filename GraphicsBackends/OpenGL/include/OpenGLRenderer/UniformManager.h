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

    inline std::unique_ptr<UniformBufferBase> createUniformBuffer(const MaterialParameterValue& value)
        {
            return std::visit([](auto&& arg) -> std::unique_ptr<UniformBufferBase>
                              {
                                  using T = std::decay_t<decltype(arg)>;

                                  if constexpr (std::is_same_v<T, float>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<float>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Vec3>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<Vec3>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Vec4>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<Vec4>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, int>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<int>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Mat4>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<Mat4>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Handle>) // Texture handle
                                  {
                                      // Textures are handled separately
                                      return nullptr;
                                  }
                                  else
                                  {
                                      return nullptr;
                                  }
                              },
                              value);
        }

    inline void setUniformValue(GLint location, const std::unique_ptr<UniformBufferBase>& uniform)
    {
        if (location == -1 || !uniform)
            return;
        switch (uniform->getType()) {
            case UniformType::FLOAT: {
                float value;
                uniform->getData(&value, sizeof(float));
                glUniform1f(location, value);
            }
                break;
            case UniformType::VEC2: {
                Vec2 value;
                uniform->getData(&value, sizeof(Vec2));
                glUniform2fv(location, 1, &value[0]);
            }
                break;
            case UniformType::VEC3: {
                Vec3 value;
                uniform->getData(&value, sizeof(Vec3));
                glUniform3fv(location, 1, &value[0]);
            }
                break;
            case UniformType::VEC4: {
                Vec4 value;
                uniform->getData(&value, sizeof(Vec4));
                glUniform4fv(location, 1, &value[0]);
            }
                break;
            case UniformType::MAT4: {
                Mat4 value;
                uniform->getData(&value, sizeof(Mat4));
                glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
            }
                break;
            case UniformType::INT: {
                int value;
                uniform->getData(&value, sizeof(int));
                glUniform1i(location, value);
            }
                break;
            case UniformType::BOOL: {
                bool value;
                uniform->getData(&value, sizeof(bool));
                glUniform1i(location, value ? 1 : 0);
            }
                break;
            default:
                break;
        }
    }

    class UniformManager
    {
    public:
        UniformManager() = default;

        ~UniformManager()
        {
            for (auto& [id, uniformBuffer] : m_uniformBuffers) {
                if (uniformBuffer->ubo != 0) {
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
            for (auto& u : uniforms) {
                auto openglBuff = std::make_unique<OpenGLUniformBuffer>();
                openglBuff->size = u->getSize();
                openglBuff->data.resize(openglBuff->size);
                u->getData(openglBuff->data.data(), u->getSize());

                glGenBuffers(1, &openglBuff->ubo);
                glBindBuffer(GL_UNIFORM_BUFFER, openglBuff->ubo);
                glBufferData(GL_UNIFORM_BUFFER,
                             openglBuff->size,
                             openglBuff->data.data(),
                             GL_DYNAMIC_DRAW);
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
            if (auto it = m_uniformBuffers.find(uniform.getId()); it != m_uniformBuffers.end()) {
                return it->second.get();
            }
            return nullptr;
        }

    private:
        CMap<GLuint, std::unique_ptr<OpenGLUniformBuffer>> m_uniformBuffers;
    };
}