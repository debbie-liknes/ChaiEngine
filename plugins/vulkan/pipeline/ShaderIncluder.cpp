#include "ShaderIncluder.h"

#include "../VulkanCommon.h"

#include <filesystem>
#include "ShaderCompiler.h"

namespace chai::gfx
{
    shaderc_include_result* ShaderIncluder::GetInclude(const char* requested_source,
                                                       shaderc_include_type type,
                                                       const char* requesting_source,
                                                       size_t include_depth)
    {
        namespace fs = std::filesystem;

        fs::path requestingPath(requesting_source);
        fs::path requestedPath(requested_source);
        fs::path resolvedPath = requestingPath.parent_path() / requestedPath;

        if (type == shaderc_include_type_standard) {
            resolvedPath = fs::weakly_canonical(shaderDir() / "common" / requestedPath);
        } else {
            resolvedPath = fs::weakly_canonical(resolvedPath);
        }
        std::string pathStr = resolvedPath.generic_string();

        auto* data = new IncludeData{pathStr, {}};

        if (!includedPaths_.insert(pathStr).second) {
            data->content = "\n";
            auto* result = new shaderc_include_result{data->path.c_str(),
                                                      data->path.size(),
                                                      data->content.c_str(),
                                                      data->content.size(),
                                                      data};
            return result;
        }

        data->content = ShaderCompiler::readFile(resolvedPath);
        if (data->content.empty() || data->content.back() != '\n')
            data->content += '\n';

        auto* result = new shaderc_include_result{data->path.c_str(),
                                                  data->path.size(),
                                                  data->content.c_str(),
                                                  data->content.size(),
                                                  data};
        return result;
    }

    void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
    {
        delete static_cast<IncludeData*>(data->user_data);
        delete data;
    }
} // namespace chai::gfx