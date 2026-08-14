#include <OS/FileIO.h>
#include <fstream>
#include <Log.h>

namespace chai
{
    std::vector<uint8_t> readFileBytes(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            CHAI_LOG_ERROR("Failed to open file: {}", path.string());
            return {};
        }

        const std::streamsize size = file.tellg();
        if (size <= 0)
            return {};

        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            CHAI_LOG_ERROR("Failed to read file: {}", path.string());
            return {};
        }

        return buffer;
    }
}