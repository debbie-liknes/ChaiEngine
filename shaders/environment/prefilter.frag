#version 450

#include <math/sampling.glsl>
#include <math/constants.glsl>

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform samplerCube envMap;
layout(push_constant) uniform Push { int faceIndex; float roughness; } pc;

vec3 dirForFace(int face, vec2 uv) {
    switch (face) {
        case 0: return normalize(vec3(  1.0, -uv.y, -uv.x)); // +X
        case 1: return normalize(vec3( -1.0, -uv.y,  uv.x)); // -X
        case 2: return normalize(vec3( uv.x,   1.0,  uv.y)); // +Y
        case 3: return normalize(vec3( uv.x,  -1.0, -uv.y)); // -Y
        case 4: return normalize(vec3( uv.x, -uv.y,   1.0)); // +Z
        default:return normalize(vec3(-uv.x, -uv.y,  -1.0)); // -Z
    }
}

void main() {
    vec3 N = dirForFace(pc.faceIndex, vUV);
    // Epics approximation. assume view == reflection == normal
    vec3 R = N;
    vec3 V = N;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefiltered = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, pc.roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            prefiltered += texture(envMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefiltered /= totalWeight;
    outColor = vec4(prefiltered, 1.0);
}