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
    vec4 baseColor;  // baseColorFactor
    vec4 emissive;   // emissiveFactor in .rgb
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
} light;

layout(set = 3, binding = 1) uniform samplerCube irradianceMap;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

vec3 getNormal()
{
    vec3 n = texture(normalTex, vUV).xyz * 2.0 - 1.0;
    vec3 N = normalize(vNormal);
    float tlen = length(vTangent.xyz);
    if (tlen < 1e-4) //check if there is a usable tangent
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

vec3 acesFilm(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    //A lot of math

    vec4 base = texture(baseColorTex, vUV) * mat.baseColor;
    if (base.a < mat.alphaCutoff) discard;
    vec3 albedo = base.rgb;
    //outColor = vec4(albedo, 1.0); return;

    // glTF packs: metallic in B, roughness in G (linear texture)
    vec3 mr = texture(metalRoughTex, vUV).rgb;
    float metallic = mr.b * mat.metallic;
    float roughness = clamp(mr.g * mat.roughness, 0.04, 1.0);

    float ao = texture(occlusionTex, vUV).r;
    vec3 emissive = texture(emissiveTex, vUV).rgb * mat.emissive.rgb;

    vec3 N = getNormal();
    if (!gl_FrontFacing) N = -N;

    vec3 V = normalize(cam.position - vWorldPos);
    vec3 L = normalize(-light.direction.xyz); // surface -> light
    vec3 H = normalize(V + L);

    float NoV = max(dot(N, V), 1e-4);
    float NoL = max(dot(N, L), 0.0);
    float NoH = max(dot(N, H), 0.0);
    float VoH = max(dot(V, H), 0.0);

    vec3 f0 = mix(vec3(0.04), albedo, metallic);
    float a = roughness * roughness;

    float D = D_GGX(NoH, a);
    float G = G_Smith(NoV, NoL, roughness);
    vec3 F = F_Schlick(VoH, f0);

    vec3 spec = (D * G * F) / max(4.0 * NoV * NoL, 1e-4);
    vec3 kd = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 diffuse = kd * albedo / PI;

    vec3 radiance = light.color.rgb;
    vec3 lo = (diffuse + spec) * radiance * NoL;

    //vec3 ambient = vec3(0.04, 0.045, 0.06) * albedo * ao;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * albedo;
    vec3 ambient    = diffuseIBL * ao;

    vec3 color = ambient + lo + emissive;

    color *= light.color.w;
    color = acesFilm(color);
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, base.a);

}
