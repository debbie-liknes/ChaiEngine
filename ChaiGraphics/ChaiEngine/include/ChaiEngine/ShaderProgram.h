#pragma once
#include <vector>
#include <Asset/AssetLoader.h>

namespace chai
{
    class ShaderAsset : public IAsset
    {
    public:
		bool isValid() const override { return m_valid; }
		const std::string& getAssetId() const override { return m_assetId; }

    private:
        std::vector<uint32_t> m_spirvCode;  // SPIR-V binary code
		// Additional metadata can be added here
		std::string m_entryPoint = "main";
		std::string m_filename;
    };
    //class ShaderProgram
    //{
    //public:
    //    uint32_t getProgramId() const { return m_programId; }

    //    // Reflection data
    //    const std::vector<UniformInfo>& getUniforms() const { return m_uniforms; }
    //    const std::vector<TextureBinding>& getTextureBindings() const { return m_textures; }

    //private:
    //    uint32_t m_programId;  // OpenGL program / Vulkan shader module
    //    std::vector<UniformInfo> m_uniforms;  // Reflected from shader
    //    std::vector<TextureBinding> m_textures;

    //    friend class ShaderCache;
    //};
}