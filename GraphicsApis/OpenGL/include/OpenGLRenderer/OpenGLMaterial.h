#pragma once
#include <ChaiEngine/IMaterial.h>
#include <glad/gl.h>
#include <vector>

namespace chai::brew
{
    class OpenGLMaterial : public IMaterial 
    {
    public:
        OpenGLMaterial(GLuint shader = 0) : shaderProgram(shader) {}

        void Bind() const override {
            if (shaderProgram) {
                glUseProgram(shaderProgram);
            }

            // Bind textures
            for (size_t i = 0; i < textures.size(); ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, textures[i]);
            }
        }

        void Unbind() const override {
            glUseProgram(0);
            for (size_t i = 0; i < textures.size(); ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        void SetMatrix4(const std::string& name, const float* matrix) override {
            GLint loc = GetUniformLocation(name);
            if (loc != -1) {
                glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
            }
        }

        void SetVector3(const std::string& name, float x, float y, float z) override {
            GLint loc = GetUniformLocation(name);
            if (loc != -1) {
                glUniform3f(loc, x, y, z);
            }
        }

        void SetVector4(const std::string& name, float x, float y, float z, float w) override {
            GLint loc = GetUniformLocation(name);
            if (loc != -1) {
                glUniform4f(loc, x, y, z, w);
            }
        }

        void SetFloat(const std::string& name, float value) override {
            GLint loc = GetUniformLocation(name);
            if (loc != -1) {
                glUniform1f(loc, value);
            }
        }

        void SetInt(const std::string& name, int value) override {
            GLint loc = GetUniformLocation(name);
            if (loc != -1) {
                glUniform1i(loc, value);
            }
        }

        void SetTexture(const std::string& name, unsigned int textureSlot) override {
            SetInt(name, textureSlot);

            // Ensure we have enough texture slots
            if (textureSlot >= textures.size()) {
                textures.resize(textureSlot + 1, 0);
            }
        }

        void AddTexture(GLuint textureID) {
            textures.push_back(textureID);
        }

    private:
        GLint GetUniformLocation(const std::string& name) const {
            if (!shaderProgram) return -1;
            return glGetUniformLocation(shaderProgram, name.c_str());
        }

        GLuint shaderProgram = 0;
        std::vector<GLuint> textures;
        bool batchMode = false;
    };
} // namespace chai::brew