#version 420 core

const float PI = 3.14159256;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

layout(std140, binding = 0) uniform PerFrameUniforms
{
    mat4 u_view;
    mat4 u_projection;
    mat4 u_invView;
};


struct ShaderLightData {
    vec4 positionAndType;// xyz = pos/dir, w = type
    vec4 color;// rgb = color, a = intensity
    vec4 directionAndRange;// xyz = direction, w = range
    vec4 spotParams;// x = inner cone, y = outer cone
};

layout(std140, binding = 2) uniform LightingData {
    ShaderLightData lights[8];
    int numLights;
} u_Lighting;

uniform vec3 u_albedo;
layout(binding = 1) uniform sampler2D u_albedoMap;
uniform float u_metallic;
layout(binding = 2) uniform sampler2D u_metallicMap;
uniform float u_roughness;
layout(binding = 3) uniform sampler2D u_roughnessMap;
//layout(binding = 4) uniform sampler2D u_normalMap;

float normalDistGGX(vec3 N, vec3 H, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH = max(dot(N, H), 0.0);
    float d = (NdotH * NdotH) * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * d * d);
}

float geometrySchlickGGX(float NdotV, float k) {
    float d = NdotV * (1.0 - k) + k;
    return NdotV / d;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float k) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float g1 = geometrySchlickGGX(NdotV, k);
    float g2 = geometrySchlickGGX(NdotL, k);

    return g1 * g2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // based on this explanation: https://learnopengl.com/PBR/Theory

    vec3 cameraPos = u_invView[3].xyz;

    vec3 albedo = texture(u_albedoMap, v_TexCoord).rgb * u_albedo;
    float metallic = texture(u_metallicMap, v_TexCoord).r * u_metallic;
    float roughness = texture(u_roughnessMap, v_TexCoord).r * u_roughness;

    vec3 N = normalize(v_Normal);
    vec3 V = normalize(cameraPos - v_FragPos);

    vec3 ambient = vec3(0.03) * albedo;
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < u_Lighting.numLights; i++) {
        vec3 L = normalize(u_Lighting.lights[i].positionAndType.xyz - v_FragPos);
        vec3 H = normalize(V + L);
        
        float NdotL = max(dot(N, L), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float HdotV = max(dot(H, V), 0.0);
        
        float D = normalDistGGX(N, H, roughness);
        float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
        float G = geometrySmith(N, V, L, k);
        vec3 F = fresnelSchlick(HdotV, F0);
        
        vec3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 0.001);
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedo / PI;
        
        vec3 radiance = u_Lighting.lights[i].color.rgb * u_Lighting.lights[i].color.a;
        Lo += (diffuse + specular) * radiance * NdotL;
    }
    FragColor = vec4(ambient + Lo, 1.0);
}