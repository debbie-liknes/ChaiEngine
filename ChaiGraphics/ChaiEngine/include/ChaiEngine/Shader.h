#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace chai_graphics
{
	class CHAIGRAPHICS_EXPORT Shader
	{
    public:
        virtual ~Shader() = default;

        // Generalized interface
        virtual void compile(const std::string& vertexSource, const std::string& fragmentSource) = 0;
        virtual void setUniform(const std::string& name, float value) = 0;
        virtual void setUniform(const std::string& name, const std::vector<float>& values) = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        // Factory method
        static Shader* create(bool useVulkan);

    protected:
        std::unordered_map<std::string, int> uniformLocations;
	};
}