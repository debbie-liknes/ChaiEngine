#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace chai_graphics
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
		virtual void Bind() = 0;

		//static std::shared_ptr<Shader> CreateFromFile(const std::string& path);

		//create from string but ONLY for opengl?

    protected:
        std::unordered_map<std::string, int> uniformLocations;
	};
}