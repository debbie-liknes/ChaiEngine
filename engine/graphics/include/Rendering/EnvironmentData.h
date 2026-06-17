/**
 * @file EnvironmentData.h
 */
#pragma once
#include <Handle.h>
#include <Assets/TextureAsset.h>

namespace chai::gfx
{
    /**
     * @brief GPU ready data for the environment
     */
    struct EnvironmentData {
        Handle<Texture> skyboxCube;
        Handle<Texture> irradiance;
        Handle<Texture> prefiltered;
        Handle<Texture> brdfLut;
        float exposure = 1.0f;
    };
}