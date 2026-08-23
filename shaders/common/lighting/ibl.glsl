#include <lighting/types/surface.glsl>

// ---------------------------------------------------------------------
// Image-based lighting
// ---------------------------------------------------------------------
vec3 CalculateIBL(SurfaceData s, vec3 R, samplerCube irradianceTex,
                   samplerCube prefilterTex_, sampler2D brdfLutTex, int mipCount)
{
    float NoV = max(dot(s.N, s.V), 0.02);
    vec3  f0  = mix(vec3(0.04), s.albedo, s.metallic);

    vec3 F  = F_SchlickRoughness(NoV, f0, s.roughness);
    vec3 kD = (vec3(1.0) - F) * (1.0 - s.metallic);

    vec3 irradiance = texture(irradianceTex, s.N).rgb;
    vec3 diffuseIBL = irradiance * s.albedo;

    vec3 prefiltered = textureLod(prefilterTex_, R, s.roughness * float(mipCount - 1)).rgb;
    vec2 brdf        = texture(brdfLutTex, vec2(NoV, s.roughness)).rg;
    vec3 specularIBL = prefiltered * (F * brdf.x + brdf.y);

    return (kD * diffuseIBL + specularIBL) * s.ao;
}