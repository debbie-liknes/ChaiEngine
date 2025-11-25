#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Material textures (set by applyMaterialState)
uniform sampler2D albedoMap;
uniform sampler2D normalMap;  // Optional
uniform sampler2D roughnessMap;  // Optional

// Material properties
uniform vec3 albedoTint;
uniform float roughness;
uniform float metallic;

// Lighting (UBO binding 2)
layout(std140, binding = 2) uniform Lighting {
    ShaderLightData lights[8];
    int numLights;
} uLighting;

void main()
{
    // Sample textures
    vec4 albedo = texture(albedoMap, TexCoord);
    vec3 baseColor = albedo.rgb * albedoTint;

    // Simple lighting (Lambertian)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));
    float NdotL = max(dot(normal, lightDir), 0.0);

    vec3 color = baseColor * (0.2 + 0.8 * NdotL);  // Ambient + diffuse

    FragColor = vec4(color, albedo.a);
}