#version 450

#include <math/sampling.glsl>
#include <math/constants.glsl>
#include <lighting/pbr.glsl>

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 outColor;

//BRDF LUT based on https://learnopengl.com/PBR/IBL/Specular-IBL

void main() {
    vec2 uv = vUV;
    float NoV = uv.x;
    float NoV2 = NoV * NoV;
    float roughness = uv.y;

    vec3 V = vec3(sqrt(1 - NoV2), 0, NoV);
    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    float A = 0.0;   
    float B = 0.0;   
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float VoH = max(dot(V, H), 0.0);
        float NoH = max(dot(N, H), 0.0);
        float NoV = max(dot(N, V), 0.0);
        float NoL = max(dot(N, L), 0.0);

        if(NoL > 0.0)
        {
            float g = G_Smith_IBL(NoV, NoL, roughness);
            float G_Vis = (g * VoH) / max(NoH * NoV, 1e-4);
            float Fc = pow(1.0 - VoH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= SAMPLE_COUNT;
    B /= SAMPLE_COUNT;

    outColor = vec4(A, B, 0.0, 1.0);
}