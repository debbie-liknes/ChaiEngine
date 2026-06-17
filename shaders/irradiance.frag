#version 450
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform samplerCube envMap;

layout(push_constant) uniform Push { int faceIndex; } pc;

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

void main() {
    vec2 uv = /* TODO */ vec2(0.0);

    vec3 N = dirForFace(pc.faceIndex, uv);

    // Tangent basis around N
    vec3 up    = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    // Convolve the hemisphere
    vec3 irradiance = vec3(0.0);
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // spherical -> tangent-space cartesian
            vec3 t = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent -> world
            vec3 sampleVec = t.x * right + t.y * up + t.z * N;
            irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples  += 1.0;
        }
    }
    irradiance = PI * irradiance / nrSamples;

    outColor = vec4(irradiance, 1.0);
}