#version 450
layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform samplerCube envMap;
layout(push_constant) uniform Push { int faceIndex; float roughness; } pc;

const float PI = 3.14159265359;

vec3 dirForFace(int face, vec2 uv) {
    switch (face) {
        case 0: return normalize(vec3(  1.0, -uv.y, -uv.x)); // +X
        case 1: return normalize(vec3( -1.0, -uv.y,  uv.x)); // -X
        case 2: return normalize(vec3( uv.x,   1.0,  uv.y)); // +Y
        case 3: return normalize(vec3( uv.x,  -1.0, -uv.y)); // -Y
        case 4: return normalize(vec3( uv.x, -uv.y,   1.0)); // +Z
        default:return normalize(vec3(-uv.x, -uv.y,  -1.0)); // -Z
    }
}

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

void main() {
    vec3 N = dirForFace(pc.faceIndex, vUV);
    // Epics approximation. assume view == reflection == normal
    vec3 R = N;
    vec3 V = N;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefiltered = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, pc.roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            prefiltered += texture(envMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefiltered /= totalWeight;
    outColor = vec4(prefiltered, 1.0);
}