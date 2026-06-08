/**
 * @file SystemPaths.h
 * @brief Free function helpers to deal with pathing across different Operating systems
 */
#pragma once
#include <filesystem>

namespace chai
{
    std::filesystem::path executableDir();
}