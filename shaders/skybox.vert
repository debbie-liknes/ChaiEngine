#version 420 core

layout(location = 0) in vec3 a_Position;

out vec3 v_Dir;

layout(std140, binding = 0) uniform PerFrame
{
    mat4 view;
    mat4 projection;
    mat4 u_invView;
};

void main()
{
    mat4 viewRot = mat4(mat3(view));

    vec4 pos = projection * viewRot * vec4(a_Position, 1.0);

    gl_Position = vec4(pos.xy, pos.w, pos.w);

    v_Dir = a_Position;
}
