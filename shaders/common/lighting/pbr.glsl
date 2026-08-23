// pbr.glsl
#include <math/constants.glsl>

// ---------------------------------------------------------------------
// Fresnel
// ---------------------------------------------------------------------

// Base Fresnel term for direct lighting
vec3 F_Schlick(float VoH, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}

// Roughness variant for IBL: as roughness increses, widen fresnel
vec3 F_SchlickRoughness(float cosTheta, vec3 f0, float roughness)
{
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ---------------------------------------------------------------------
// Geometry
// ---------------------------------------------------------------------

float G_SchlickGGX(float NdotV, float k)
{
    return NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
}

// Direct lighting remap
float G_Smith_Direct(float NoV, float NoL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return G_SchlickGGX(NoV, k) * G_SchlickGGX(NoL, k);
}

// IBL remap - this is different
float G_Smith_IBL(float NoV, float NoL, float roughness)
{
    float a = roughness * roughness;
    float k = (a * a) / 2.0;
    return G_SchlickGGX(NoV, k) * G_SchlickGGX(NoL, k);
}


// ---------------------------------------------------------------------
// Normal distribution function
// ---------------------------------------------------------------------

float D_GGX(float NoH, float a)
{
    float a2 = a * a;
    float d  = NoH * NoH * (a2 - 1.0) + 1.0;
    return a2 / max(PI * d * d, EPSILON);
}
