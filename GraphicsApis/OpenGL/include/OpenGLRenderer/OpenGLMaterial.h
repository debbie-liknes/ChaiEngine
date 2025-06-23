#pragma once
#include <ChaiEngine/IMaterial.h>
#include <glad/gl.h>
#include <vector>
#include <unordered_map>

namespace chai::brew
{
    struct OpenGLMaterialData 
    {
        GLuint shaderProgram = 0;
        std::unordered_map<std::string, GLint> uniformLocations;
        bool isCompiled = false;
    };
    //class OpenGLMaterial : public IMaterial 
    //{
    //public:
    //    OpenGLMaterial(const std::string& vertexShader, const std::string& fragmentShader);
    //    ~OpenGLMaterial() override;

    //    // Material interface
    //    void bind() const override;
    //    void unbind() const override;

    //    void setUniform(const std::string& name, const UniformBufferBase& value) override;
    //    bool hasUniform(const std::string& name) const override;

    //    void setTexture(const std::string& samplerName, uint32_t textureId, int slot = 0) override;
    //    void removeTexture(const std::string& samplerName) override;

    //    uint32_t getShaderId() const override { return shaderProgram; }
    //    bool isValid() const override { return shaderProgram != 0; }

    //    bool isTransparent() const override { return transparent; }
    //    int getRenderQueue() const override { return renderQueue; }

    //    // OpenGL specific
    //    void setTransparent(bool value) { transparent = value; }
    //    void setRenderQueue(int queue) { renderQueue = queue; }

    //private:
    //    GLuint compileShader(const std::string& source, GLenum type);
    //    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    //    void cacheUniformLocations();
    //    void applyUniform(const std::string& name, const UniformBufferBase& value) const;
    //    std::string loadShaderFromFile(const std::string& filepath);

    //    GLuint shaderProgram;
    //    std::unordered_map<std::string, GLint> uniformLocations;
    //    std::unordered_map<std::string, const UniformBufferBase&> uniformValues;
    //    std::vector<TextureSlot> textures;
    //    bool transparent;
    //    int renderQueue;
    //};
} // namespace chai::brew