/**
 * @file Primitives.h
 * @brief Create MeshAssets for common primitive types
 */
#pragma once
#include <MeshAsset.h>

namespace chai::gfx
{
    /**
     * @brief Cube primitive asset
     */
    MeshAsset makeCube(float size);
}