#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <Resource/ResourceLoader.h>

namespace chai::brew
{
	enum PrimitiveMode
	{
		POINTS,
		LINES,
		TRIANGLES
	};

	enum CHAIGRAPHICS_EXPORT ShaderStage
	{
		VERTEX,
		FRAGMENT,
		NONE
	};

	struct CHAIGRAPHICS_EXPORT ShaderData
	{
		std::string shaderSource = "";
		ShaderStage stage = ShaderStage::NONE;
	};

	class CHAIGRAPHICS_EXPORT Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() = 0;
		virtual bool isBound() = 0;
		std::string shaderSource;
	};

	class CHAIGRAPHICS_EXPORT ShaderResource : public IResource
	{
	public:
		virtual ~ShaderResource() = default;
		const std::string& GetName() const
		{
			return m_name;
		}

		std::string m_name;
		ShaderStage shaderStage;
		std::string filePath;
		std::shared_ptr<Shader> shader;
	};
}