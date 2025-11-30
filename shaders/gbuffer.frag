#version 420 core

in vec3 v_FragPos;
in vec2 v_TexCoord;
in mat3 v_TBN;

// G-Buffer outputs - one for each render target
layout(location = 0) out vec3 gPosition;   // RGB16F
layout(location = 1) out vec3 gNormal;     // RGB16F
layout(location = 2) out vec4 gAlbedo;     // RGBA8
layout(location = 3) out vec2 gMaterial;   // RG8

// Material uniforms - same names as your PBR shader
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;

layout(binding = 0) uniform sampler2D u_albedoMap;
layout(binding = 1) uniform sampler2D u_metallicRoughnessMap;
layout(binding = 2) uniform sampler2D u_normalMap;

void main()
{
    gPosition = v_FragPos;
    
    // Normal handling
    vec3 normalTex = texture(u_normalMap, v_TexCoord).rgb;
    
    if (length(normalTex) > 0.1 && normalTex != vec3(1.0)) {
        normalTex = normalTex * 2.0 - 1.0;
        gNormal = normalize(v_TBN * normalTex);
    } else {
        gNormal = normalize(v_TBN[2]);
    }
    
    gAlbedo.rgb = texture(u_albedoMap, v_TexCoord).rgb * u_albedo;
    gAlbedo.a = 1.0;
    
    vec4 mr = texture(u_metallicRoughnessMap, v_TexCoord);
    
    // Clamp material values to valid PBR ranges
    float metallic = clamp(mr.b * u_metallic, 0.0, 1.0);
    float roughness = clamp(mr.g * u_roughness, 0.04, 1.0);  // Min 0.04 to avoid specular issues
    
    //gMaterial.r = metallic;
    //gMaterial.g = roughness;

    gMaterial.r = clamp(metallic, 0.0, 1.0);
    gMaterial.g = clamp(roughness, 0.04, 1.0);
}