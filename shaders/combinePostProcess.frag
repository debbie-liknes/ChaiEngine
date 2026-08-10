#version 450
layout(location = 0) in vec2 vUV;

layout (location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;
layout(set = 0, binding = 1) uniform sampler2D bloomColor;

void main()
{
    vec3 scene = texture(sceneColor, vUV).rgb;
    vec3 bloom = texture(bloomColor, vUV).rgb;
    FragColor = vec4(scene + bloom * 1.5, 1.0); // the 1.0 is bloom intensity, can prob use a uniform later
}