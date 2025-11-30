#version 420 core

const float PI = 3.14159265;

in vec2 v_TexCoord;
out vec4 FragColor;

// G-Buffer textures
layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedo;
layout(binding = 3) uniform sampler2D gMaterial;

layout(std140, binding = 0) uniform PerFrameUniforms
{
    mat4 u_view;
    mat4 u_projection;
    mat4 u_invView;
};

struct ShaderLightData {
    vec4 positionAndType;
    vec4 color;
    vec4 directionAndRange;
    vec4 spotParams;
};

layout(std140, binding = 2) uniform LightingData {
    ShaderLightData lights[8];
    int numLights;
} u_Lighting;

// PBR functions
float normalDistGGX(vec3 N, vec3 H, float roughness)
{
    // Ensure minimum roughness
    roughness = max(roughness, 0.08);
    
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = dot(N, H);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float geometrySchlickGGX(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return geometrySchlickGGX(NdotV, k) * geometrySchlickGGX(NdotL, k);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Sample G-Buffer
    vec3 fragPos = texture(gPosition, v_TexCoord).rgb;
    vec3 N = normalize(texture(gNormal, v_TexCoord).rgb);
    vec3 albedo = texture(gAlbedo, v_TexCoord).rgb;
    vec2 material = texture(gMaterial, v_TexCoord).rg;
    float metallic = material.r;
    float roughness = material.g;
    roughness = max(roughness, 0.04);
    
    // Skip empty pixels
    //if (fragPos == vec3(0.0) && N == vec3(0.0)) {
    //    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    //    return;
    //}

    if (length(fragPos) < 0.001 || length(N) < 0.001) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // Or your sky color
        return;
    }
    
    vec3 cameraPos = u_invView[3].xyz;
    vec3 V = normalize(cameraPos - fragPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < u_Lighting.numLights; i++) {
        vec3 lightPos = u_Lighting.lights[i].positionAndType.xyz;
        float lightType = u_Lighting.lights[i].positionAndType.w;
        vec3 lightColor = u_Lighting.lights[i].color.rgb;
        float lightIntensity = u_Lighting.lights[i].color.a;
        float lightRange = u_Lighting.lights[i].directionAndRange.w;
        
        vec3 L;
        float attenuation = 1.0;
        
        if (lightType == 0.0) {
            // Directional
            L = normalize(-u_Lighting.lights[i].directionAndRange.xyz);
        } else {
            // Point/Spot
            vec3 toLight = lightPos - fragPos;
            float distance = length(toLight);
            L = toLight / distance;
            
            float falloff = clamp(1.0 - pow(distance / lightRange, 4.0), 0.0, 1.0);
            attenuation = (falloff * falloff) / (distance * distance + 1.0);
            
            // Spot light cone
            if (lightType == 2.0) {
                vec3 spotDir = normalize(u_Lighting.lights[i].directionAndRange.xyz);
                float innerCone = u_Lighting.lights[i].spotParams.x;
                float outerCone = u_Lighting.lights[i].spotParams.y;
                float theta = dot(L, -spotDir);
                float epsilon = innerCone - outerCone;
                attenuation *= clamp((theta - outerCone) / epsilon, 0.0, 1.0);
            }
        }
        
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float HdotV = max(dot(H, V), 0.0);
        
        float D = normalDistGGX(N, H, roughness);
        float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
        float G = geometrySmith(N, V, L, k);
        vec3 F = fresnelSchlick(HdotV, F0);
        
        vec3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 0.001);
        specular = min(specular, vec3(10.0));
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedo / PI;
        
        vec3 radiance = lightColor * lightIntensity * attenuation;
        Lo += (diffuse + specular) * radiance * NdotL;
    }
    
    vec3 ambient = albedo / PI;

    // Debug: if scene is too dark, uncomment to check G-Buffer
    //FragColor = vec4(albedo, 1.0);  // Should show colors
    //FragColor = vec4(N * 0.5 + 0.5, 1.0);  // Should show normals as colors
    //FragColor = vec4(vec3(metallic), 1.0);  // Should show metallic
    //FragColor = vec4(vec3(roughness), 1.0);  // Should show roughness
    FragColor = vec4(ambient + Lo, 1.0);
    //vec3 L = normalize(vec3(1, 1, 1));
    //vec3 H = normalize(V + L);
    //float D = normalDistGGX(N, H, roughness);
    //FragColor = vec4(vec3(D * 0.1), 1.0);
}