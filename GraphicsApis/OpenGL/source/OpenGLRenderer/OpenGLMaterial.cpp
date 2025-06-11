#include <OpenGLRenderer/OpenGLMaterial.h>
#include <iostream>
#include <sstream>
#include <ChaiEngine/UniformBuffer.h>

namespace chai::brew
{
    OpenGLMaterial::OpenGLMaterial(const std::string& vertexShader, const std::string& fragmentShader)
        : shaderProgram(0), transparent(false), renderQueue(1000) {

        // Compile shaders
        GLuint vertexShaderId = compileShader(vertexShader, GL_VERTEX_SHADER);
        GLuint fragmentShaderId = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

        if (vertexShaderId && fragmentShaderId) {
            shaderProgram = linkProgram(vertexShaderId, fragmentShaderId);
            if (shaderProgram) {
                cacheUniformLocations();
            }
        }

        // Clean up individual shaders
        if (vertexShaderId) glDeleteShader(vertexShaderId);
        if (fragmentShaderId) glDeleteShader(fragmentShaderId);
    }

    OpenGLMaterial::~OpenGLMaterial() {
        if (shaderProgram) {
            glDeleteProgram(shaderProgram);
        }
    }

    void OpenGLMaterial::bind() const {
        if (!isValid()) {
            std::cerr << "Warning: Attempting to bind invalid material\n";
            return;
        }

        glUseProgram(shaderProgram);

        // Apply all cached uniforms
        for (const auto& [name, value] : uniformValues) {
            applyUniform(name, value);
        }

        // Bind textures
        for (const auto& texture : textures) {
            glActiveTexture(GL_TEXTURE0 + texture.slot);
            glBindTexture(GL_TEXTURE_2D, texture.textureId);

            // Set sampler uniform
            GLint location = glGetUniformLocation(shaderProgram, texture.samplerName.c_str());
            if (location != -1) {
                glUniform1i(location, texture.slot);
            }
        }
    }

    void OpenGLMaterial::unbind() const {
        glUseProgram(0);

        // Unbind textures
        for (const auto& texture : textures) {
            glActiveTexture(GL_TEXTURE0 + texture.slot);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glActiveTexture(GL_TEXTURE0); // Reset to default
    }

    void OpenGLMaterial::setUniform(const std::string& name, const UniformBufferBase& value) {
        uniformValues.insert({name, value});

        // If material is currently bound, apply immediately
        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        if (currentProgram == static_cast<GLint>(shaderProgram)) {
            applyUniform(name, value);
        }
    }

    bool OpenGLMaterial::hasUniform(const std::string& name) const {
        return uniformLocations.find(name) != uniformLocations.end();
    }

    void OpenGLMaterial::setTexture(const std::string& samplerName, uint32_t textureId, int slot) {
        // Remove existing texture with same sampler name
        removeTexture(samplerName);

        // Add new texture
        TextureSlot newTexture;
        newTexture.textureId = textureId;
        newTexture.slot = slot;
        newTexture.samplerName = samplerName;

        textures.push_back(newTexture);
    }

    void OpenGLMaterial::removeTexture(const std::string& samplerName) {
        textures.erase(
            std::remove_if(textures.begin(), textures.end(),
                [&samplerName](const TextureSlot& slot) {
                    return slot.samplerName == samplerName;
                }),
            textures.end());
    }

    GLuint OpenGLMaterial::compileShader(const std::string& source, GLenum type) {
        GLuint shader = glCreateShader(type);
        const char* sourceCStr = source.c_str();

        glShaderSource(shader, 1, &sourceCStr, nullptr);
        glCompileShader(shader);

        // Check compilation status
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);

            const char* shaderTypeStr = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
            std::cerr << "Shader compilation failed (" << shaderTypeStr << "):\n" << infoLog << std::endl;

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint OpenGLMaterial::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
        GLuint program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        // Check linking status
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success) {
            GLchar infoLog[1024];
            glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Program linking failed:\n" << infoLog << std::endl;

            glDeleteProgram(program);
            return 0;
        }

        return program;
    }

    void OpenGLMaterial::cacheUniformLocations() {
        if (!shaderProgram) return;

        GLint uniformCount;
        glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &uniformCount);

        for (GLint i = 0; i < uniformCount; ++i) {
            GLchar name[256];
            GLsizei length;
            GLint size;
            GLenum type;

            glGetActiveUniform(shaderProgram, i, sizeof(name), &length, &size, &type, name);
            GLint location = glGetUniformLocation(shaderProgram, name);

            if (location != -1) {
                uniformLocations[std::string(name)] = location;
            }
        }
    }

    void OpenGLMaterial::applyUniform(const std::string& name, const UniformBufferBase& value) const {
        auto it = uniformLocations.find(name);
        if (it == uniformLocations.end()) {
            return; // Uniform not found, silently ignore
        }

        GLint location = it->second;

        //std::visit([location](const auto& val) {
        //    using T = std::decay_t<decltype(val)>;

        //    if constexpr (std::is_same_v<T, int>) {
        //        glUniform1i(location, val);
        //    }
        //    else if constexpr (std::is_same_v<T, float>) {
        //        glUniform1f(location, val);
        //    }
        //    else if constexpr (std::is_same_v<T, std::array<float, 2>>) {
        //        glUniform2fv(location, 1, val.data());
        //    }
        //    else if constexpr (std::is_same_v<T, std::array<float, 3>>) {
        //        glUniform3fv(location, 1, val.data());
        //    }
        //    else if constexpr (std::is_same_v<T, std::array<float, 4>>) {
        //        glUniform4fv(location, 1, val.data());
        //    }
        //    else if constexpr (std::is_same_v<T, std::array<float, 16>>) {
        //        glUniformMatrix4fv(location, 1, GL_FALSE, val.data());
        //    }
        //    }, value);
    }

    std::string OpenGLMaterial::loadShaderFromFile(const std::string& filepath) {
        //std::ifstream file(filepath);
        //if (!file.is_open()) {
        //    std::cerr << "Failed to open shader file: " << filepath << std::endl;
        //    return "";
        //}

        std::stringstream buffer;
        //buffer << file.rdbuf();
        return buffer.str();
    }
}