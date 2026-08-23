// material_pbr.glsl

struct MaterialData {
    vec4 baseColor;
    vec4 emissive;
    float metallic;
    float roughness;
    float alphaCutoff;
    float _pad;
};