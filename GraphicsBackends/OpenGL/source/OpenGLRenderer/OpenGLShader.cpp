#include <OpenGLRenderer/OpenGLShader.h>
#include <OpenGLRenderer/GLHelpers.h>

namespace chai::brew
{
	//std::string GLShaderManager::generateShaderHash(std::shared_ptr<ShaderDescription> shaderDesc,
	//	const std::set<MaterialFeature>& features) 
	//{
	//	using enum chai::MaterialFeature;
	//	std::stringstream hash;

	//	// Include shader name/path
	//	hash << shaderDesc->name;

	//	// Include all shader stage paths (vertex, fragment, etc.)
	//	for (const auto& stage : shaderDesc->stages)
	//	{
	//		hash << "_" << stage.path;
	//	}

	//	// Include feature flags that affect compilation
	//	if (features.contains(BaseColorTexture))
	//		hash << "_BC";
	//	if (features.contains(NormalTexture))
	//		hash << "_NM";
	//	if (features.contains(MetallicTexture))
	//		hash << "_MT";
	//	if (features.contains(RoughnessTexture))
	//		hash << "_RG";
	//	if (features.contains(EmissionTexture))
	//		hash << "_EM";
	//	if (features.contains(Transparency))
	//		hash << "_TR";
	//	if (features.contains(DoubleSided))
	//		hash << "_DS";

	//	return hash.str();
	//}

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

		GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
		checkGLError("compile vertex shader");
		GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
		checkGLError("compile frag shader");

		if (vertexShader == 0 || fragmentShader == 0)
		{
			return 0;
		}

		GLuint program = glCreateProgram();
		checkGLError("create shader program");
		glAttachShader(program, vertexShader);
		checkGLError("attach shader");
		glAttachShader(program, fragmentShader);
		checkGLError("attach shader");
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
			return 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		m_programToShaderData[program] = new OpenGLShaderData();
		m_programToShaderData[program]->program = program;

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