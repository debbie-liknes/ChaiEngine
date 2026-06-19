#version 450

layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vTangent;
layout(location = 3) in vec2 vUV;

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
    float bias = 0.001;
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range

    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy, currentDepth)).r; 

    float shadow = 0.0;
    vec2 texel = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y) {
            float d = texture(shadowMap,
            vec3(
                projCoords.xy + vec2(x, y) * texel,
                currentDepth - bias));
            shadow += (currentDepth - bias) > d ? 1.0 : 0.0;
        }
    shadow /= 9.0;

    return shadow;
}  

void main()
{
    vec4 base = texture(baseColorTex, vUV) * mat.baseColor;
    if (base.a < mat.alphaCutoff) discard;
    vec3 albedo = base.rgb;

    vec3 mr = texture(metalRoughTex, vUV).rgb;
    float metallic  = mr.b * mat.metallic;
    float roughness = clamp(mr.g * mat.roughness, 0.04, 1.0);
    float ao        = texture(occlusionTex, vUV).r;
    vec3 emissive   = texture(emissiveTex, vUV).rgb * mat.emissive.rgb;

    vec3 N = getNormal();
    if (!gl_FrontFacing) N = -N;
    vec3 V = normalize(cam.position - vWorldPos);
    vec3 R = reflect(-V, N);
    vec3 L = normalize(-light.direction.xyz);
    vec3 H = normalize(V + L);

    float NoV = max(dot(N, V), 1e-4);
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
    vec3  radiance = light.color.rgb;

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
    vec3 color = ambient + lo + emissive;
    color *= light.color.w;
    color = acesFilm(color);
    color = pow(color, vec3(1.0 / 2.2));
    outColor = vec4(color, base.a);
}
