// lighting_ops.glsl
#pragma once

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

vec3 CalculateDirectLighting(vec3 N, vec3 V, vec3 L, vec3 radiance,
                                 vec3 albedo, float metallic, float roughness)
{
    vec3 H = normalize(V + L);
    float a  = roughness * roughness;
    vec3  f0 = mix(vec3(0.04), albedo, metallic);

    float NoV = max(dot(N, V), 0.02); 
    float NoL = max(dot(N, L), 0.0);
    float NoH = max(dot(N, H), 0.0);
    float VoH = max(dot(V, H), 0.0);

    // Cook-Torrance
    float D = D_GGX(NoH, a);
    float G = G_Smith(NoV, NoL, roughness);
    vec3  Fd = F_Schlick(VoH, f0);
    vec3  spec = (D * G * Fd) / max(4.0 * NoV * NoL, 1e-4);
    vec3  kdDirect = (vec3(1.0) - Fd) * (1.0 - metallic);
    vec3  diffuseDirect = kdDirect * albedo / PI;

    return diffuseDirect;
}