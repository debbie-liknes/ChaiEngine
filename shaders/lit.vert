#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inTangent;

layout(push_constant) uniform Push { mat4 model; vec4 color; } pc;

layout(set = 0, binding = 0) uniform Camera { 
    mat4 view;
    mat4 proj;
    mat4 viewProj;    //precomputed for convenience
    vec3 position;    //world space eye
    float _pad0;
} cam;

// Pass data to the fragment shader
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUV;

void main() {
    gl_Position = cam.viewProj * pc.model * vec4(inPosition, 1.0);
    fragNormal = mat3(pc.model) * inNormal;
    fragUV = inUV;
}