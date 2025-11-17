#include <ChaiEngine/Material.h>
#include <ChaiEngine/UniformBuffer.h>

namespace chai
{
	//PipelineDesc Material::createMainPassPipeline()
	//{
 //       PipelineDesc desc;

 //       // Shaders
 //       desc.shaderStages = {
 //           {ShaderStage::Type::Vertex, "shaders/pbr.vert"},
 //           {ShaderStage::Type::Fragment, "shaders/pbr.frag"}
 //       };

 //       // Features ? Defines
 //       if (hasFeature(MaterialFeature::BaseColorTexture)) {
 //           desc.globalDefines.push_back("HAS_BASE_COLOR_TEXTURE");
 //       }

 //       // Features ? Pipeline State
 //       desc.rasterizer.cullMode = hasFeature(MaterialFeature::DoubleSided)
 //           ? RasterizerState::CullMode::None
 //           : RasterizerState::CullMode::Back;

 //       desc.blend = hasFeature(MaterialFeature::Transparency)
 //           ? BlendState::alphaBlend()
 //           : BlendState::opaque();

 //       desc.vertexLayout = VertexLayout::positionNormalUV();

 //       return desc;
	//}

	//PipelineDesc Material::createShadowPassPipeline()
	//{
 //       PipelineDesc desc;

 //       desc.shaderStages = {
 //           {ShaderStage::Type::Vertex, "shaders/shadow.vert"},
 //           // No fragment shader - depth only!
 //       };

 //       desc.rasterizer.cullMode = RasterizerState::CullMode::Front;  // Fix peter-panning
 //       desc.rasterizer.depthBiasEnable = true;
 //       desc.rasterizer.depthBiasConstant = 1.25f;
 //       desc.rasterizer.depthBiasSlope = 1.75f;

 //       desc.blend.colorWriteMask = BlendState::ColorWriteMask::None;  // No color output!
 //       desc.depthStencil.depthWriteEnable = true;

 //       desc.vertexLayout = VertexLayout::positionNormalUV();

 //       return desc;
	//}
}