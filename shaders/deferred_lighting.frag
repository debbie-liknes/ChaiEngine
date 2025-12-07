#version 420 core

const float PI = 3.14159265;
const int NUM_CASCADES = 4;

in vec2 v_TexCoord;
out vec4 FragColor;

// G-Buffer textures
layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedo;
layout(binding = 3) uniform sampler2D gMaterial;

// CSM - 4 cascades per light, up to 2 lights = 8 shadow maps
uniform sampler2D u_shadowMaps[8];
uniform mat4 u_lightViewProjs[8];
uniform vec4 u_cascadeSplits;  // Split depths for 4 cascades

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

int getCascadeIndex(float viewDepth) {
    for (int i = 0; i < NUM_CASCADES; i++) {
        if (viewDepth < u_cascadeSplits[i]) {
            return i;
        }
    }
    return NUM_CASCADES - 1;
}

float getShadowCSM(int lightIndex, vec3 worldPos, vec3 normal, vec3 lightDir, float viewDepth) {
    int cascadeIndex = getCascadeIndex(viewDepth);
    int shadowMapIndex = lightIndex * NUM_CASCADES + cascadeIndex;

    vec4 lightSpacePos = u_lightViewProjs[shadowMapIndex] * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 1.0;

    // Slope-scaled bias - smaller for closer cascades
    float baseBias = 0.005 * (cascadeIndex + 1);
    float bias = max(baseBias * (1.0 - dot(normal, lightDir)), baseBias * 0.5);

    float currentDepth = projCoords.z;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMaps[shadowMapIndex], 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float depth = texture(u_shadowMaps[shadowMapIndex], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > depth ? 0.0 : 1.0;
        }
    }

    return shadow / 9.0;
}

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
    vec3 fragPos = texture(gPosition, v_TexCoord).rgb;
    vec3 N = normalize(texture(gNormal, v_TexCoord).rgb);
    vec3 albedo = texture(gAlbedo, v_TexCoord).rgb;
    vec2 material = texture(gMaterial, v_TexCoord).rg;
    float metallic = material.r;
    float roughness = max(material.g, 0.04);

    if (length(fragPos) < 0.001 || length(N) < 0.001) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Calculate view depth for cascade selection
    vec4 viewPos = u_view * vec4(fragPos, 1.0);
    float viewDepth = -viewPos.z;  // Positive depth

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
        float shadow = 1.0;

        if (lightType == 0.0) {
            // Directional - use CSM
            L = normalize(-u_Lighting.lights[i].directionAndRange.xyz);
            shadow = getShadowCSM(i, fragPos, N, L, viewDepth);
        } else {
            // Point/Spot
            vec3 toLight = lightPos - fragPos;
            float distance = length(toLight);
            L = toLight / distance;

            float falloff = clamp(1.0 - pow(distance / lightRange, 4.0), 0.0, 1.0);
            attenuation = (falloff * falloff) / (distance * distance + 1.0);

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
        Lo += shadow * (diffuse + specular) * radiance * NdotL;
    }

    //vec3 ambient = albedo / PI * 0.03;
    vec3 ambient = albedo / PI;
    FragColor = vec4(ambient + Lo, 1.0);

    //int cascadeIndex = getCascadeIndex(viewDepth);
    //vec3 cascadeColors[4] = vec3[](
    //vec3(1, 0, 0),
    //vec3(0, 1, 0),
    //vec3(0, 0, 1),
    //vec3(1, 1, 0)
    //);
    //FragColor = vec4(mix(cascadeColors[cascadeIndex], ambient + Lo, 0.7), 1.0);
}