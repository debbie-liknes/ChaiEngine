#version 450
layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 outColor;

//BRDF LUT based on https://learnopengl.com/PBR/IBL/Specular-IBL

const float PI = 3.14159265359;

float VanDerCorput(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for(uint i = 0u; i < 32u; ++i)
    {
        if(n > 0u)
        {
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }

    return result;
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), VanDerCorput(i, 2u));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float G_SchlickGGX_IBL(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

float G_Smith_IBL(float NoV, float NoL, float roughness)
{
    float a = roughness * roughness;
    float k = (a * a) / 2.0;
    return G_SchlickGGX_IBL(NoV, k) * G_SchlickGGX_IBL(NoL, k);
}

void main() {
    vec2 uv = vUV;
    float NoV = uv.x;
    float NoV2 = NoV * NoV;
    float roughness = uv.y;

    vec3 V = vec3(sqrt(1 - NoV2), 0, NoV);
    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    float A = 0.0;   
    float B = 0.0;   
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float VoH = max(dot(V, H), 0.0);
        float NoH = max(dot(N, H), 0.0);
        float NoV = max(dot(N, V), 0.0);
        float NoL = max(dot(N, L), 0.0);

        if(NoL > 0.0)
        {
            float g = G_Smith_IBL(NoV, NoL, roughness);
            float G_Vis = (g * VoH) / (NoH * NoV);
            float Fc = pow(1.0 - VoH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= SAMPLE_COUNT;
    B /= SAMPLE_COUNT;

    outColor = vec4(A, B, 0.0, 1.0);
}