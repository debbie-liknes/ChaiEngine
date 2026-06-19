#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inTangent; // xyz = tangent, w = handedness sign

// set 0 = camera
layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
} cam;

layout(push_constant) uniform Push {
    mat4 model;
} pc;

layout(location = 0) out vec3 vWorldPos;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec4 vTangent;
layout(location = 3) out vec2 vUV;

void main()
{
    vec4 worldPos = pc.model * vec4(inPosition, 1.0);
    vWorldPos = worldPos.xyz;

    mat3 nrmMat = mat3(transpose(inverse(pc.model)));
    vNormal = normalize(nrmMat * inNormal);
    vTangent = vec4(normalize(nrmMat * inTangent.xyz), inTangent.w);
    vUV = inUV;

    gl_Position = cam.viewProj * worldPos;
}
