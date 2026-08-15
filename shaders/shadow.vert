#version 450

layout(location = 0) in vec3 inPosition;

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
