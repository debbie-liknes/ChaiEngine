#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Shader.h>
#include <ChaiEngine/UniformBuffer.h>
#include <set>
#include <Resource/ResourceLoader.h>
#include <ChaiEngine/Renderer.h>
#include <Meta/ChaiMacros.h>

namespace chai::brew
{
	class OPENGLRENDERER_EXPORT GLShader : public Shader
	{
	public:
		GLShader();

		//the source here isnt the path, its the raw data
		void createShader(const char* source, brew::ShaderStage stage);
		void bind() override;
		bool isBound() override;

		int getHandle();
		ShaderStage getStage() { return m_stage; }

	private:
		bool m_isBound = false;
		int m_shaderHandle;
		ShaderStage m_stage;
	};

	//probably overkill for very simple shaders that are just string data
	class OPENGLRENDERER_EXPORT GLShaderLoader : public IResourceLoader {
	public:
		GLShaderLoader();
		GLShaderLoader(Renderer* renderer);

		bool CanLoad(const std::string& ext) const override;
		std::shared_ptr<IResource> Load(const std::string& path) override;

	private:
		Renderer* m_renderer;
	};

	CHAI_CLASS(chai::brew::GLShaderLoader)
	END_CHAI()
}