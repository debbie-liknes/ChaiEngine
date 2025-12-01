#version 420 core

in vec3 v_FragPos;
in vec2 v_TexCoord;
in mat3 v_TBN;

layout(location = 0) out vec3 gPosition;   // RGB16F, world-space position
layout(location = 1) out vec3 gNormal;     // RGB16F, world-space normal
layout(location = 2) out vec4 gAlbedo;     // RGBA8
layout(location = 3) out vec2 gMaterial;   // RG8

uniform vec3  u_albedo;
uniform float u_metallic;
uniform float u_roughness;

layout(binding = 0) uniform sampler2D u_albedoMap;
layout(binding = 1) uniform sampler2D u_metallicRoughnessMap;
layout(binding = 2) uniform sampler2D u_normalMap;

uniform bool u_hasNormalMap;

void main()
{
    // World-space position
    gPosition = v_FragPos;

    // Worldspace normal
    vec3 N;
    if (u_hasNormalMap) {
        // Tangent-space normal from [0,1] -> [-1,1]
        vec3 normalTex = texture(u_normalMap, v_TexCoord).rgb;
        normalTex = normalTex * 2.0 - 1.0;

        // Transform to world space
        N = normalize(v_TBN * normalTex);
    } else {
        // v_TBN must be built as mat3(T, B, N) in the vertex shader
        N = normalize(v_TBN[2]);
    }

    gNormal = N;

    // Albedo
    gAlbedo.rgb = texture(u_albedoMap, v_TexCoord).rgb * u_albedo;
    gAlbedo.a   = 1.0;

    // Metallic/Roughness
    vec4 mr = texture(u_metallicRoughnessMap, v_TexCoord);

    float metallic  = clamp(mr.b * u_metallic, 0.0, 1.0);
    float roughness = clamp(mr.g * u_roughness, 0.04, 1.0);

    gMaterial.r = metallic;
    gMaterial.g = roughness;
}
