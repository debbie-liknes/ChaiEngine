#version 450

layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vTangent;
layout(location = 3) in vec2 vUV;

layout(push_constant) uniform Push {
    mat4 model;
    int shadingMode;
} pc;

// set 0 = camera
layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
} cam;

// set 1 = material
layout(set = 1, binding = 0) uniform Material {
    vec4 baseColor;
    vec4 emissive;
    float metallic;
    float roughness;
    float alphaCutoff;
    float _pad;
} mat;
layout(set = 1, binding = 1) uniform sampler2D baseColorTex;
layout(set = 1, binding = 2) uniform sampler2D metalRoughTex;
layout(set = 1, binding = 3) uniform sampler2D normalTex;
layout(set = 1, binding = 4) uniform sampler2D occlusionTex;
layout(set = 1, binding = 5) uniform sampler2D emissiveTex;

// set 2 = light
layout(set = 2, binding = 0) uniform Light {
    vec4 direction; // .xyz = direction
    vec4 color;     // .rgb = color of light, w = stength
    mat4 view;
    mat4 lightSpaceProj;
} light;
layout(set = 2, binding = 1) uniform sampler2DShadow shadowMap;

layout(set = 3, binding = 1) uniform samplerCube irradianceMap;
layout(set = 3, binding = 2) uniform sampler2D brdfLut;
layout(set = 3, binding = 3) uniform samplerCube prefilterTex;
int prefilterMipCount = 5;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

vec3 getNormal()
{
    vec3 n = texture(normalTex, vUV).xyz * 2.0 - 1.0;
    vec3 N = normalize(vNormal);
    float tlen = length(vTangent.xyz);
    if (tlen < 1e-4)
        return N;
    vec3 T = normalize(vTangent.xyz);
    vec3 B = cross(N, T) * vTangent.w;
    return normalize(mat3(T, B, N) * n);
}

float D_GGX(float NoH, float a)
{
    float a2 = a * a;
    float d = NoH * NoH * (a2 - 1.0) + 1.0;
    return a2 / (PI * d * d);
}

// Smith with Schlick GGX
float G_Smith(float NoV, float NoL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float gv = NoV / (NoV * (1.0 - k) + k);
    float gl = NoL / (NoL * (1.0 - k) + k);
    return gv * gl;
}

vec3 F_Schlick(float VoH, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 acesFilm(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

//tells us how much of the frament is IN shadow
float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    // beyond the lights far plane, treat as fully lit
    if (projCoords.z > 1.0) return 0.0;

    float bias = 0.001;
    float ref  = projCoords.z - bias;

    float shadow = 0.0;
    vec2 texel = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y) {
            float lit = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texel, ref));
            shadow += 1.0 - lit;
        }
    return shadow / 9.0;
}

void main()
{
    vec4 base = texture(baseColorTex, vUV) * mat.baseColor;
    if (base.a < mat.alphaCutoff) discard;
    vec3 albedo = base.rgb;

    vec3 mr = texture(metalRoughTex, vUV).rgb;
    float metallic  = mr.b * mat.metallic;
    float roughness = clamp(mr.g * mat.roughness, 0.08, 1.0);
    float ao        = texture(occlusionTex, vUV).r;
    vec3 emissive   = texture(emissiveTex, vUV).rgb * mat.emissive.rgb;

    vec3 N = getNormal();
    if (!gl_FrontFacing) N = -N;
    vec3 V = normalize(cam.position - vWorldPos);
    vec3 R = reflect(-V, N);
    vec3 L = normalize(-light.direction.xyz);
    vec3 H = normalize(V + L);

    float NoV = max(dot(N, V), 0.02); 
    float NoL = max(dot(N, L), 0.0);
    float NoH = max(dot(N, H), 0.0);
    float VoH = max(dot(V, H), 0.0);

    vec3  f0 = mix(vec3(0.04), albedo, metallic);
    float a  = roughness * roughness;

    // Cook-Torrance
    float D = D_GGX(NoH, a);
    float G = G_Smith(NoV, NoL, roughness);
    vec3  Fd = F_Schlick(VoH, f0);
    vec3  spec = (D * G * Fd) / max(4.0 * NoV * NoL, 1e-4);
    vec3  kdDirect = (vec3(1.0) - Fd) * (1.0 - metallic);
    vec3  diffuseDirect = kdDirect * albedo / PI;
    vec3 radiance = light.color.rgb * light.color.w;

    // IBL (ambient)
    vec3 Fi = fresnelSchlickRoughness(NoV, f0, roughness);
    vec3 kD = (vec3(1.0) - Fi) * (1.0 - metallic);

    vec3 irradiance  = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL  = irradiance * albedo;

    vec3 prefiltered = textureLod(prefilterTex, R, roughness * float(prefilterMipCount - 1)).rgb;
    vec2 brdf        = texture(brdfLut, vec2(NoV, roughness)).rg;
    vec3 specularIBL = prefiltered * (Fi * brdf.x + brdf.y);

    vec4 lightPos = light.lightSpaceProj * light.view * vec4(vWorldPos, 1.0);
    float shadow = ShadowCalculation(lightPos); 
    //shadows attenuate lo, which is from the light (sun)
    vec3 lo = (diffuseDirect + spec) * radiance * NoL * (1.0 - shadow);
    vec3 ambient = (kD * diffuseIBL + specularIBL) * ao;

    // ---- combine ----
    //TODO: make this a uniform
    float exposure = 1.0;
    vec3 color = ambient + lo + emissive;
    color *= exposure;
    //color = vec3(0.0, 1.0, 0.0);
    //color = acesFilm(color);

    vec3 finalColor;
    switch (pc.shadingMode) {
        case 1: finalColor = normalize(N) * 0.5 + 0.5; break;
        case 2: finalColor = vec3(vUV, 0.0);           break;
        case 3: finalColor = vec3(roughness);          break;
        case 4: finalColor = vec3(metallic);           break;
        case 5: finalColor = vec3(ao);                 break;
        default: finalColor = color;                   break;
    }
    outColor = vec4(finalColor, base.a);
}
