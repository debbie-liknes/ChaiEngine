#pragma once
#include <ChaiEngine/Shader.h>

namespace chai::brew
{
    class ShaderManager {
    public:
        std::shared_ptr<Shader> Load(const std::string& name, const ShaderSource& source);
        std::shared_ptr<Shader> Get(const std::string& name) const;

        //void ReloadAll();

    private:
        std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
        std::unordered_map<std::string, ShaderSource> sources;
    };
}