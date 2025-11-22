#version 420 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

layout(std140, binding = 0) uniform PerFrameUniforms
{
    mat4 u_view;
    mat4 u_projection;
};

//uniform vec3 u_DiffuseColor;
//uniform vec3 u_SpecularColor;
//uniform float u_Shininess;

void main()
{
    vec3 u_DiffuseColor = vec3(0.0, 1.0, 0.0);
    vec3 u_SpecularColor = vec3(1.0, 1.0, 1.0);
    float u_Shininess = 1.0;
    // Hardcoded lighting for now
    vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float lightIntensity = 1.0;
    vec3 ambientColor = vec3(0.1, 0.1, 0.1);

    // Normalize normal
    vec3 normal = normalize(v_Normal);
    vec3 toLight = -lightDir;

    // Get camera position from view matrix inverse
    vec3 viewPos = vec3(inverse(u_view)[3]);
    vec3 viewDir = normalize(viewPos - v_FragPos);

    // Ambient
    vec3 ambient = ambientColor * u_DiffuseColor;

    // Diffuse
    float diff = max(dot(normal, toLight), 0.0);
    vec3 diffuse = diff * u_DiffuseColor * lightColor * lightIntensity;

    // Specular
    vec3 halfwayDir = normalize(toLight + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
    vec3 specular = spec * u_SpecularColor * lightColor * lightIntensity;

    // Final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}