#include <OpenGLRenderer/OpenGLShader.h>
#include <OpenGLRenderer/GLHelpers.h>

#include <Graphics/ShaderAsset.h>
#include <Resource/ResourceManager.h>

namespace chai::brew
{
	GLuint GLShaderManager::compileShaderFromAsset(AssetHandle shaderAssetHandle)
    {
        const auto* shaderAsset = AssetManager::instance().get<ShaderAsset>(shaderAssetHandle);
        if (!shaderAsset)
        {
            std::cerr << "Failed to get shader asset!" << std::endl;
            return 0;
        }

        // Get vertex and fragment stages
        const char* vertexSource = nullptr;
        const char* fragmentSource = nullptr;

        for (const auto& stage : shaderAsset->getStages())
        {
            if (stage.stage == ShaderStage::Vertex)
            {
                vertexSource = stage.sourceCode.c_str();
            }
            else if (stage.stage == ShaderStage::Fragment)
            {
                fragmentSource = stage.sourceCode.c_str();
            }
        }

        if (!vertexSource || !fragmentSource)
        {
            std::cerr << "Shader asset missing vertex or fragment stage!" << std::endl;
            return 0;
        }

        // Compile the program
        GLuint program = compileShaderProgram(vertexSource, fragmentSource);
        if (program == 0)
        {
            return 0;
        }

        // Create shader data
        auto shaderData = std::make_unique<OpenGLShaderData>();
        shaderData->program = program;
        shaderData->shaderAssetHandle = shaderAssetHandle;

        // Cache uniform locations (you'd get these from somewhere)
        // For now, hardcode common ones
        shaderData->uniformLocations["u_DiffuseColor"] = glGetUniformLocation(program, "u_DiffuseColor");
        shaderData->uniformLocations["u_SpecularColor"] = glGetUniformLocation(program, "u_SpecularColor");
        shaderData->uniformLocations["u_Shininess"] = glGetUniformLocation(program, "u_Shininess");

        m_programToShaderData[program] = std::move(shaderData);

        return program;
    }

	GLuint GLShaderManager::createDefaultShaderProgram()
	{
		// Simple vertex shader
		const char* vertexShaderSource = R"(
			#version 420 core

			layout(location = 0) in vec3 a_position;

			layout(std140, binding = 0) uniform PerFrame
			{
			    mat4 u_view;
			    mat4 u_proj;
			};

			layout(std140, binding = 1) uniform PerDraw
			{
			    mat4 u_model;
			    mat4 u_normal;
			};

			void main()
			{
			    gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
			}
			)";

		const char* fragmentShaderSource = R"(
			#version 330 core
			out vec4 FragColor;

			void main()
			{
			    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
			}
			)";

		GLuint program = compileShaderProgram(vertexShaderSource, fragmentShaderSource);
		if (program == 0) return 0;

		// Store shader data
		auto shaderData = std::make_unique<OpenGLShaderData>();
		shaderData->program = program;
		m_programToShaderData[program] = std::move(shaderData);

		return program;
	}

    /*// Update getOrCreatePhongShader to create a ShaderAsset
    GLuint GLShaderManager::getOrCreatePhongShader()
    {
        if (m_phongShaderProgram != 0)
        {
            return m_phongShaderProgram;
        }

        std::cout << "Creating shared Phong shader..." << std::endl;

        // Same vertex/fragment source as before...
        const char* vertexSource = R"(
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform PerFrame
{
    mat4 u_view;
    mat4 u_proj;
};

layout(std140, binding = 1) uniform PerDraw
{
    mat4 u_model;
    mat4 u_normal;
};

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    vec4 worldPos = u_model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = mat3(u_normal) * a_Normal;
    v_TexCoord = a_TexCoord;

    gl_Position = u_proj * u_view * worldPos;
}
)";

        const char* fragmentSource = R"(
#version 420 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

layout(std140) uniform PerFrameUniforms
{
    mat4 u_View;
    mat4 u_Projection;
};

uniform vec3 u_DiffuseColor;
uniform vec3 u_SpecularColor;
uniform float u_Shininess;

void main()
{
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float lightIntensity = 1.0;
    vec3 ambientColor = vec3(0.1, 0.1, 0.1);

    vec3 normal = normalize(v_Normal);
    vec3 toLight = -lightDir;

    vec3 viewPos = vec3(inverse(u_View)[3]);
    vec3 viewDir = normalize(viewPos - v_FragPos);

    vec3 ambient = ambientColor * u_DiffuseColor;

    float diff = max(dot(normal, toLight), 0.0);
    vec3 diffuse = diff * u_DiffuseColor * lightColor * lightIntensity;

    vec3 halfwayDir = normalize(toLight + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
    vec3 specular = spec * u_SpecularColor * lightColor * lightIntensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

        GLuint program = compileShaderProgram(vertexSource, fragmentSource);
        if (program == 0)
        {
            std::cerr << "Failed to create Phong shader!" << std::endl;
            return 0;
        }

        // Create a ShaderAsset for this (normally you'd load from file)
        auto phongAsset = std::make_shared<ShaderAsset>("phong");
        phongAsset->addStage({ShaderStage::Vertex, std::string(vertexSource)});
        phongAsset->addStage({ShaderStage::Fragment, std::string(fragmentSource)});

        // Declare vertex inputs (THIS IS THE KEY PART!)
        phongAsset->addVertexInput("a_Position", 0, AttributeType::Float3);
        phongAsset->addVertexInput("a_Normal", 1, AttributeType::Float3);
        phongAsset->addVertexInput("a_TexCoord", 2, AttributeType::Float2);

        // Store the asset somewhere accessible (you might want a shader asset manager)
        // For now, we'll just reference it by the program ID

        auto shaderData = std::make_unique<OpenGLShaderData>();
        shaderData->program = program;
        // Store asset handle if you have one

        shaderData->uniformLocations["u_DiffuseColor"] = glGetUniformLocation(program, "u_DiffuseColor");
        shaderData->uniformLocations["u_SpecularColor"] = glGetUniformLocation(program, "u_SpecularColor");
        shaderData->uniformLocations["u_Shininess"] = glGetUniformLocation(program, "u_Shininess");

        m_programToShaderData[program] = std::move(shaderData);
        m_phongShaderProgram = program;

        std::cout << "Phong shader created successfully (program ID: " << program << ")" << std::endl;
        return program;
    }*/

	GLuint GLShaderManager::compileShaderProgram(const char* vertexSource, const char* fragmentSource)
	{
		GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
		checkGLError("compile vertex shader");
		GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
		checkGLError("compile fragment shader");

		if (vertexShader == 0 || fragmentShader == 0)
		{
			if (vertexShader != 0) glDeleteShader(vertexShader);
			if (fragmentShader != 0) glDeleteShader(fragmentShader);
			return 0;
		}

		GLuint program = glCreateProgram();
		checkGLError("create shader program");
		glAttachShader(program, vertexShader);
		checkGLError("attach vertex shader");
		glAttachShader(program, fragmentShader);
		checkGLError("attach fragment shader");
		glLinkProgram(program);
		checkGLError("link shader program");

		// Check linking
		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cerr << "Shader program linking failed: " << infoLog << std::endl;
			glDeleteProgram(program);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}

	GLuint GLShaderManager::compileShader(const char* source, GLenum type)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		// Check compilation
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "Shader compilation failed: " << infoLog << std::endl;
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}
}
