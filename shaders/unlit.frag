#version 450
layout(push_constant) uniform Push {
    mat4 mvp;
    vec4 color;
} pc;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;
layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 0) uniform sampler2D tex;

void main() {
    outColor = texture(tex, fragUV);
}