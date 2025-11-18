#pragma once
#include <vector>

namespace chai
{
    class ShaderProgram
    {
    public:
        uint32_t getProgramId() const { return m_programId; }

        // Reflection data
        const std::vector<UniformInfo>& getUniforms() const { return m_uniforms; }
        const std::vector<TextureBinding>& getTextureBindings() const { return m_textures; }

    private:
        uint32_t m_programId;  // OpenGL program / Vulkan shader module
        std::vector<UniformInfo> m_uniforms;  // Reflected from shader
        std::vector<TextureBinding> m_textures;

        friend class ShaderCache;
    };
}