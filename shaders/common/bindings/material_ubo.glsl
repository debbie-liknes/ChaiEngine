// material_ubo.glsl
#pragma once

struct MaterialData {
    vec4 baseColor;
    vec4 emissive;
    float metallic;
    float roughness;
    float alphaCutoff;
    float _pad;
};