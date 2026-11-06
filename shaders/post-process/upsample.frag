#version 450
layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 0) uniform sampler2D srcTex;
layout(push_constant) uniform PC { vec2 srcTexelSize; } pc;

void main() {
    vec2 uv = vUV;
    vec2 o = pc.srcTexelSize;

    vec3 s = texture(srcTex, uv + vec2(-o.x, -o.y)).rgb * 1.0
           + texture(srcTex, uv + vec2( 0.0, -o.y)).rgb * 2.0
           + texture(srcTex, uv + vec2( o.x, -o.y)).rgb * 1.0
           + texture(srcTex, uv + vec2(-o.x,  0.0)).rgb * 2.0
           + texture(srcTex, uv).rgb                    * 4.0
           + texture(srcTex, uv + vec2( o.x,  0.0)).rgb * 2.0
           + texture(srcTex, uv + vec2(-o.x,  o.y)).rgb * 1.0
           + texture(srcTex, uv + vec2( 0.0,  o.y)).rgb * 2.0
           + texture(srcTex, uv + vec2( o.x,  o.y)).rgb * 1.0;

    FragColor = vec4(s / 16.0, 1.0);
}