// lighting_ops.glsl
#include <lighting/types/surface.glsl>
#include <lighting/pbr.glsl>

//tells us how much of the frament is IN shadow
float ShadowCalculation(vec4 fragPosLightSpace, sampler2DShadow shadowMap)
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

// ---------------------------------------------------------------------
// Direct light. Cook Torrance specular + Lambertian diffuse
// ---------------------------------------------------------------------
vec3 CalculateDirectLighting(SurfaceData s, vec3 L, vec3 radiance, float shadow)
{
    vec3 H = normalize(s.V + L);

    float NoV = max(dot(s.N, s.V), 0.02);
    float NoL = max(dot(s.N, L), 0.0);
    float NoH = max(dot(s.N, H), 0.0);
    float VoH = max(dot(s.V, H), 0.0);

    vec3  f0 = mix(vec3(0.04), s.albedo, s.metallic);
    float a  = s.roughness * s.roughness;

    float D  = D_GGX(NoH, a);
    float G  = G_Smith_Direct(NoV, NoL, s.roughness);
    vec3  F  = F_Schlick(VoH, f0);
    vec3  spec = (D * G * F) / max(4.0 * NoV * NoL, 1e-4);

    vec3  kd = (vec3(1.0) - F) * (1.0 - s.metallic);
    vec3  diffuse = kd * s.albedo / PI;

    return (diffuse + spec) * radiance * NoL * (1.0 - shadow);
}