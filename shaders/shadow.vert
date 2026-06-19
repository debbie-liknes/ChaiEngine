#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inTangent;

layout(push_constant) uniform Push {
    mat4 lightProj;
    mat4 view;
    mat4 model;
} pc;

void main()
{
    //this puts the position in light space
    gl_Position = pc.lightProj * pc.view * pc.model * vec4(inPosition, 1.0);
}
