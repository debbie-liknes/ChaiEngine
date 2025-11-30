#version 420 core

in vec3 v_FragPos;
in vec2 v_TexCoord;
in mat3 v_TBN;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec2 gMaterial;

uniform vec3 u_albedo;
layout(binding = 1) uniform sampler2D u_albedoMap;
uniform float u_metallic;
uniform float u_roughness;
layout(binding = 2) uniform sampler2D u_metallicRoughnessMap;
layout(binding = 3) uniform sampler2D u_normalMap;
uniform bool u_hasNormalMap;

void main()
{
    gPosition = v_FragPos;
    
    // Normal
    if (u_hasNormalMap) {
        vec3 normalTex = texture(u_normalMap, v_TexCoord).rgb * 2.0 - 1.0;
        gNormal = normalize(v_TBN * normalTex);
    } else {
        gNormal = normalize(v_TBN[2]);
    }
    
    // Albedo
    gAlbedo.rgb = texture(u_albedoMap, v_TexCoord).rgb * u_albedo;
    gAlbedo.a = 1.0;
    
    // Material properties
    vec4 mr = texture(u_metallicRoughnessMap, v_TexCoord);
    gMaterial.r = mr.b * u_metallic;
    gMaterial.g = mr.g * u_roughness;
}