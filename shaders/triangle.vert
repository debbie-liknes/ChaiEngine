#version 450

// Vertex inputs — locations MUST match vertexAttributes() in Mesh.h
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inTangent;

// Push constant — must match VkPushConstantRange (vertex stage, 64 bytes = one mat4)
layout(push_constant) uniform Push {
    mat4 mvp;
} pc;

// Pass data to the fragment shader
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUV;

void main() {
    gl_Position = pc.mvp * vec4(inPosition, 1.0);
    fragNormal  = inNormal;
    fragUV      = inUV;
}