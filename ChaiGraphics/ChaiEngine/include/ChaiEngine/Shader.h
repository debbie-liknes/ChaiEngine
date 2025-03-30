#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

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
		//static std::shared_ptr<Shader> createFromFile(const std::string& path);
		//static std::shared_ptr<Shader> createFromSource(const std::string& shaderData);
	};
}