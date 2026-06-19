#pragma once
#include <vector>
#include <filesystem>

namespace chai
{
    std::vector<uint8_t> readFileBytes(const std::filesystem::path& path);
}