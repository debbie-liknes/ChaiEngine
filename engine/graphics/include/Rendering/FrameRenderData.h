/**
 * @file FrameRenderData.h
 * @brief Structures used when extracting from scene objects
 */
#pragma once
#include <GraphicsExport.h>
#include <ChaiMath.h>
#include <Handle.h>
#include <Rendering/LightData.h>
#include <Rendering/EnvironmentData.h>

namespace chai::gfx
{
    struct Mesh;
    struct Texture;
    struct Material;

    struct RenderView {
        math::Mat4 view;
        math::Vec3 position;
        float fovYRadians;
        float nearPlane;
        float farPlane;
        uint32_t cameraId;
    };

    /**
     * @brief Represents a single drawable for the renderer
     * The model mat is fully transformed to world space, according to its
     * hierarchy
     */
    struct RenderItem {
        Handle<Mesh> mesh;
        math::Mat4 model;
        Handle<Material> material;
    };

    /**
     * @brief Extract data from the scene into this. This structure is handed to the
     * renderer every frame
     */
    struct FrameRenderData {
        std::vector<RenderView> views;
        std::vector<RenderItem> items;
        LightData sun; // have 1 directional light, TODO: rethink after IBL
        EnvironmentData environment;
    };
}