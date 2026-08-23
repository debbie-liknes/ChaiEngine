#version 450

#include <lighting/types/lights.glsl>
#include <lighting/types/material_pbr.glsl>
#include <lighting/pbr.glsl>
#include <lighting/lighting_ops.glsl>
#include <lighting/ibl.glsl>
#include <camera/camera.glsl>

// input from vert shader
layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vTangent;
layout(location = 3) in vec2 vUV;

layout(push_constant) uniform Push {
    mat4 model;
    int shadingMode;
} pc;

//uniform bindings

// set 0 = camera
layout(set = 0, binding = 0) uniform Camera { CameraData data; } cam;

// set 1 = material
layout(set = 1, binding = 0) uniform Material { MaterialData data; } mat;
layout(set = 1, binding = 1) uniform sampler2D baseColorTex;
layout(set = 1, binding = 2) uniform sampler2D metalRoughTex;
layout(set = 1, binding = 3) uniform sampler2D normalTex;
layout(set = 1, binding = 4) uniform sampler2D occlusionTex;
layout(set = 1, binding = 5) uniform sampler2D emissiveTex;

layout(set = 2, binding = 0) uniform Light { LightData data; } light;
layout(set = 2, binding = 1) uniform sampler2DShadow shadowMap;

layout(set = 3, binding = 1) uniform samplerCube irradianceMap;
layout(set = 3, binding = 2) uniform sampler2D brdfLut;
layout(set = 3, binding = 3) uniform samplerCube prefilterTex;
int prefilterMipCount = 5;

layout(location = 0) out vec4 outColor;

// ---------------------------------------------------------------------
// Sample all material textures
// ---------------------------------------------------------------------
void SampleMaterial(vec2 uv, out vec4 baseColor, out float metallic,
                     out float roughness, out float ao, out vec3 emissive)
{
    vec4 base = texture(baseColorTex, uv) * mat.data.baseColor;
    baseColor = base;

    vec3 mr  = texture(metalRoughTex, uv).rgb;
    metallic  = mr.b * mat.data.metallic;
    roughness = clamp(mr.g * mat.data.roughness, 0.08, 1.0);
    ao        = texture(occlusionTex, uv).r;
    emissive  = texture(emissiveTex, uv).rgb * mat.data.emissive.rgb;
}

// ---------------------------------------------------------------------
// Tangent-space normal map -> world space
// ---------------------------------------------------------------------
vec3 GetNormal(vec2 uv, vec3 vertexNormal, vec4 tangent)
{
    vec3 n = texture(normalTex, uv).xyz * 2.0 - 1.0;
    vec3 N = normalize(vertexNormal);
    float tlen = length(tangent.xyz);
    if (tlen < EPSILON)
        return N;
    vec3 T = normalize(tangent.xyz);
    vec3 B = cross(N, T) * tangent.w;
    return normalize(mat3(T, B, N) * n);
}

// Debug shading override view
vec3 ApplyShadingModeOverride(int mode, vec3 litColor, vec3 N, vec2 uv,
                               float roughness, float metallic, float ao)
{
    switch (mode) {
        case 1: return normalize(N) * 0.5 + 0.5;
        case 2: return vec3(uv, 0.0);
        case 3: return vec3(roughness);
        case 4: return vec3(metallic);
        case 5: return vec3(ao);
        default: return litColor;
    }
}

void main()
{
    vec4  baseColor;
    float metallic, roughness, ao;
    vec3  emissive;
    SampleMaterial(vUV, baseColor, metallic, roughness, ao, emissive);
    if (baseColor.a < mat.data.alphaCutoff) discard;

    vec3 N = GetNormal(vUV, vNormal, vTangent);
    if (!gl_FrontFacing) N = -N;
    vec3 V = normalize(cam.data.position - vWorldPos);
    vec3 R = reflect(-V, N);
    vec3 L = normalize(-light.data.direction.xyz);

    SurfaceData surface;
    surface.albedo    = baseColor.rgb;
    surface.metallic  = metallic;
    surface.roughness = roughness;
    surface.ao        = ao;
    surface.N         = N;
    surface.V         = V;

    vec3 radiance = light.data.color.rgb * light.data.color.w;
    vec4 lightSpacePos = light.data.lightSpaceProj * light.data.view * vec4(vWorldPos, 1.0);
    float shadow = ShadowCalculation(lightSpacePos, shadowMap);

    vec3 directLighting = CalculateDirectLighting(surface, L, radiance, shadow);
    vec3 ambientLighting = CalculateIBL(surface, R, irradianceMap, prefilterTex,
                                         brdfLut, prefilterMipCount);

    // ---- combine ----
    vec3 color = (ambientLighting + directLighting + emissive);

    vec3 finalColor = ApplyShadingModeOverride(pc.shadingMode, color, N, vUV,
                                                roughness, metallic, ao);
    outColor = vec4(finalColor, baseColor.a);
}
