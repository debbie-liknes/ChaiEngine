#include <OpenGLRenderer/OpenGLRenderer.h>
#include <iostream>
#include <algorithm>
#include <ChaiEngine/RenderCommandList.h>
#include <ChaiEngine/UniformBuffer.h>
#include <ChaiEngine/Material.h>
#include <fstream>
#include <sstream>

namespace chai::brew
{
	const char* getGLErrorString(GLenum err) 
	{
		switch (err) 
		{
		case GL_NO_ERROR: return "No error";
		case GL_INVALID_ENUM: return "Invalid enum";
		case GL_INVALID_VALUE: return "Invalid value";
		case GL_INVALID_OPERATION: return "Invalid operation";
		case GL_STACK_OVERFLOW: return "Stack overflow";
		case GL_STACK_UNDERFLOW: return "Stack underflow";
		case GL_OUT_OF_MEMORY: return "Out of memory";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
		default: return "Unknown error";
		}
	}

	static void checkGLError(const std::string& context)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) 
		{
			std::cerr << "OpenGL Error (" << context << "): " << getGLErrorString(err) << std::endl;
		}
	}

	bool OpenGLBackend::initialize(void* winProcAddress)
	{
		if (!gladLoadGL((GLADloadfunc)winProcAddress))
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			checkGLError("setProcAddress");
			return false;
		}

		checkGLError("setProcAddress");

		// Check OpenGL version
		const auto* version = (const char*)glGetString(GL_VERSION);
		std::cout << "OpenGL Version: " << version << std::endl;

		// Set default OpenGL state
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		// Enable blending for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		defaultShaderProgram = createDefaultShaderProgram();
		if (defaultShaderProgram == 0) 
		{
			std::cerr << "Failed to create default shader program" << std::endl;
			return false;
		}

		return true;
	}

	GLuint OpenGLBackend::createDefaultShaderProgram()
	{
		// Simple vertex shader
		const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_transform;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_transform * vec4(aPos, 1.0);
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

		return program;
	}

	GLuint OpenGLBackend::compileShader(const char* source, GLenum type)
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

	void OpenGLBackend::shutdown()
	{
		m_meshCache.clear();
		m_materialCache.clear();

		if (defaultShaderProgram) 
		{
			glDeleteProgram(defaultShaderProgram);
			defaultShaderProgram = 0;
		}
	}

	void OpenGLBackend::executeCommands(const std::vector<RenderCommand>& commands)
	{
		for (const auto& cmd : commands) 
		{
			switch (cmd.type) 
			{
			case RenderCommand::DRAW_MESH:
				drawMesh(cmd);
				break;
			case RenderCommand::SET_VIEWPORT:
				int x, y, width, height;
				cmd.viewport->getViewport(x, y, width, height);
				glViewport(x, y, width, height);
				checkGLError("glViewport");
				break;
			case RenderCommand::CLEAR:
				//this should come from RenderCommand
				clear(0.0f, 0.0f, 0.0f, 1.0f);
				checkGLError("clear window");
				break;
			case RenderCommand::SET_SCISSOR:
				// SetScissor(cmd.scissor.x, cmd.scissor.y, cmd.scissor.width, cmd.scissor.height);
				break;
			case RenderCommand::SET_LIGHTS:
				m_cachedLights = cmd.lights; // Cache lights for later use
				break;
			}
		}
	}

	void OpenGLBackend::clear(float r, float g, float b, float a) 
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLBackend::drawMesh(const RenderCommand& cmd)
	{
		//check if mesh handle is valid
		if (!cmd.mesh.isValid())
		{
			std::cerr << "Invalid mesh handle in drawMesh command" << std::endl;
			return;
		}

		// Get or create OpenGL mesh data
		OpenGLMeshData* glMeshData = getOrCreateMeshData(cmd.mesh);

		// Upload mesh if not already uploaded
		if (!glMeshData->isUploaded)
		{
			uploadMeshToGPU(cmd.mesh, glMeshData);
		}

		// Handle material
		GLuint shaderToUse = defaultShaderProgram;
		OpenGLMaterialData* glMaterialData = nullptr;

		if (cmd.material)
		{
			glMaterialData = getOrCreateMaterialData(cmd.material);

			// Compile material shader if needed
			if (!glMaterialData->isCompiled) 
			{
				compileMaterial(cmd.material, glMaterialData);
			}

			if (glMaterialData->shaderProgram && glMaterialData->shaderProgram != 0) 
			{
				shaderToUse = glMaterialData->shaderProgram;
			}
		}

		// Bind shader
		bindShaderProgram(shaderToUse);

		if (m_lightsDirty)
		{
			setLightsUniforms(); // Update lights if they are dirty
			m_lightsDirty = false;
		}

		// Apply material state to OpenGL
		if (cmd.material && glMaterialData) 
		{
			applyMaterialState(cmd.material, shaderToUse, glMaterialData);
		}

		// Set built-in transform uniforms
		setBuiltinUniforms(shaderToUse, cmd);

		// Bind and draw mesh
		bindVertexArray(glMeshData->VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(glMeshData->indexCount), GL_UNSIGNED_INT, nullptr);
		checkGLError("draw mesh");

		// Clean up texture bindings
		unbindTextures(glMaterialData);
	}

	void OpenGLBackend::setLightsUniforms()
	{
		// Assuming you have a current shader program bound
		GLint lightCountLoc = glGetUniformLocation(currentShaderProgram, "u_lightCount");
		glUniform1i(lightCountLoc, (GLint)m_cachedLights.size());

		for (size_t i = 0; i < m_cachedLights.size() && i < 16; ++i)
		{
			std::string base = "u_lights[" + std::to_string(i) + "]";

			GLint typeLoc = glGetUniformLocation(currentShaderProgram, (base + ".type").c_str());
			GLint posLoc = glGetUniformLocation(currentShaderProgram, (base + ".position").c_str());
			GLint dirLoc = glGetUniformLocation(currentShaderProgram, (base + ".direction").c_str());
			GLint colorLoc = glGetUniformLocation(currentShaderProgram, (base + ".color").c_str());
			GLint intensityLoc = glGetUniformLocation(currentShaderProgram, (base + ".intensity").c_str());
			GLint rangeLoc = glGetUniformLocation(currentShaderProgram, (base + ".range").c_str());
			GLint attenuationLoc = glGetUniformLocation(currentShaderProgram, (base + ".attenuation").c_str());
			GLint innerConeLoc = glGetUniformLocation(currentShaderProgram, (base + ".innerCone").c_str());
			GLint outerConeLoc = glGetUniformLocation(currentShaderProgram, (base + ".outerCone").c_str());
			GLint enabledLoc = glGetUniformLocation(currentShaderProgram, (base + ".enabled").c_str());

			const Light& light = m_cachedLights[i];
			glUniform1i(typeLoc, light.type);
			glUniform3fv(posLoc, 1, &light.position[0]);
			glUniform3fv(dirLoc, 1, &light.direction[0]);
			glUniform3fv(colorLoc, 1, &light.color[0]);
			glUniform1f(intensityLoc, light.intensity);
			glUniform1f(rangeLoc, light.range);
			glUniform3fv(attenuationLoc, 1, &light.attenuation[0]);
			glUniform1f(innerConeLoc, light.innerCone);
			glUniform1f(outerConeLoc, light.outerCone);
			glUniform1i(enabledLoc, light.enabled);
		}
	}

	void OpenGLBackend::unbindTextures(OpenGLMaterialData* glMaterialData)
	{
		if (!glMaterialData) return;

		// Unbind all textures that were bound by this material
		for (int slot : glMaterialData->boundTextures) 
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Reset to texture unit 0
		glActiveTexture(GL_TEXTURE0);
	}

	void OpenGLBackend::setBuiltinUniforms(GLuint shaderProgram, const RenderCommand& cmd)
	{
		// Set transform matrix uniform
		if (GLint transformLoc = glGetUniformLocation(shaderProgram, "u_transform"); transformLoc != -1) 
		{
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &cmd.transform[0][0]);
		}

		if (GLint viewLoc = glGetUniformLocation(shaderProgram, "u_view"); viewLoc != -1) 
		{
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &cmd.viewMatrix[0][0]);
		}

		GLint projLoc = glGetUniformLocation(shaderProgram, "u_projection");
		if (projLoc != -1) 
		{
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, &cmd.projectionMatrix[0][0]);
		}
	}

	void OpenGLBackend::applyMaterialState(IMaterial* material, GLuint shaderProgram, OpenGLMaterialData* glMaterialData)
	{
		auto const* concreteMaterial = static_cast<Material*>(material);
		// Apply uniforms from the material
		const auto& uniforms = concreteMaterial->getUniforms();
		for (const auto& [name, uniform] : uniforms) 
		{
			setUniformFromData(shaderProgram, name, *uniform);
		}

		// Apply textures from the material
		const auto& textures = concreteMaterial->getTextures();
		int textureSlot = 0;

		// Clear previous texture bindings
		glMaterialData->boundTextures.clear();

		for (const auto& [samplerName, textureInfo] : textures) 
		{
			// Bind texture to slot
			glActiveTexture(GL_TEXTURE0 + textureSlot);
			glBindTexture(GL_TEXTURE_2D, textureInfo.id);

			// Set sampler uniform
			if (GLint samplerLoc = glGetUniformLocation(shaderProgram, samplerName.c_str()); samplerLoc != -1) 
			{
				glUniform1i(samplerLoc, textureSlot);
			}

			// Track bound textures for cleanup
			glMaterialData->boundTextures.push_back(textureSlot);
			textureSlot++;
		}

		// Set material feature flags as uniforms (for shaders that need runtime checks)
		const auto& features = concreteMaterial->getEnabledFeatures();

		if (GLint hasBaseColorTexLoc = glGetUniformLocation(shaderProgram, "u_hasBaseColorTexture"); hasBaseColorTexLoc != -1) 
		{
			glUniform1i(hasBaseColorTexLoc, features.contains(MaterialFeature::BaseColorTexture) ? 1 : 0);
		}

		if (GLint hasNormalTexLoc = glGetUniformLocation(shaderProgram, "u_hasNormalTexture"); hasNormalTexLoc != -1) 
		{
			glUniform1i(hasNormalTexLoc, features.contains(MaterialFeature::NormalTexture) ? 1 : 0);
		}

		if (GLint hasMetallicTexLoc = glGetUniformLocation(shaderProgram, "u_hasMetallicTexture"); hasMetallicTexLoc != -1) 
		{
			glUniform1i(hasMetallicTexLoc, features.contains(MaterialFeature::MetallicTexture) ? 1 : 0);
		}

		if (GLint hasRoughnessTexLoc = glGetUniformLocation(shaderProgram, "u_hasRoughnessTexture"); hasRoughnessTexLoc != -1) 
		{
			glUniform1i(hasRoughnessTexLoc, features.contains(MaterialFeature::RoughnessTexture) ? 1 : 0);
		}

		// Handle transparency rendering state
		if (features.contains(MaterialFeature::Transparency)) 
		{
			// Enable blending if not already enabled
			if (!glIsEnabled(GL_BLEND)) 
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}

		// Handle double-sided rendering
		if (features.contains(MaterialFeature::DoubleSided)) 
		{
			glDisable(GL_CULL_FACE);
		}
		else 
		{
			glEnable(GL_CULL_FACE);
		}
	}

	void OpenGLBackend::cacheUniformLocations(GLuint shaderProgram, OpenGLMaterialData* glMaterialData)
	{
		// Cache common uniform locations
		glMaterialData->uniformLocations["u_transform"] = glGetUniformLocation(shaderProgram, "u_transform");
		glMaterialData->uniformLocations["u_view"] = glGetUniformLocation(shaderProgram, "u_view");
		glMaterialData->uniformLocations["u_projection"] = glGetUniformLocation(shaderProgram, "u_projection");

		// PBR uniforms
		glMaterialData->uniformLocations["u_baseColor"] = glGetUniformLocation(shaderProgram, "u_baseColor");
		glMaterialData->uniformLocations["u_metallic"] = glGetUniformLocation(shaderProgram, "u_metallic");
		glMaterialData->uniformLocations["u_roughness"] = glGetUniformLocation(shaderProgram, "u_roughness");
		glMaterialData->uniformLocations["u_emission"] = glGetUniformLocation(shaderProgram, "u_emission");
		glMaterialData->uniformLocations["u_transparency"] = glGetUniformLocation(shaderProgram, "u_transparency");

		// Legacy Phong uniforms
		glMaterialData->uniformLocations["u_diffuse"] = glGetUniformLocation(shaderProgram, "u_diffuse");
		glMaterialData->uniformLocations["u_specular"] = glGetUniformLocation(shaderProgram, "u_specular");
		glMaterialData->uniformLocations["u_ambient"] = glGetUniformLocation(shaderProgram, "u_ambient");
		glMaterialData->uniformLocations["u_shininess"] = glGetUniformLocation(shaderProgram, "u_shininess");

		// Texture samplers
		glMaterialData->uniformLocations["u_baseColorTexture"] = glGetUniformLocation(shaderProgram, "u_baseColorTexture");
		glMaterialData->uniformLocations["u_normalTexture"] = glGetUniformLocation(shaderProgram, "u_normalTexture");
		glMaterialData->uniformLocations["u_metallicTexture"] = glGetUniformLocation(shaderProgram, "u_metallicTexture");
		glMaterialData->uniformLocations["u_roughnessTexture"] = glGetUniformLocation(shaderProgram, "u_roughnessTexture");
		glMaterialData->uniformLocations["u_emissionTexture"] = glGetUniformLocation(shaderProgram, "u_emissionTexture");

		// Feature flags
		glMaterialData->uniformLocations["u_hasBaseColorTexture"] = glGetUniformLocation(shaderProgram, "u_hasBaseColorTexture");
		glMaterialData->uniformLocations["u_hasNormalTexture"] = glGetUniformLocation(shaderProgram, "u_hasNormalTexture");
		glMaterialData->uniformLocations["u_hasMetallicTexture"] = glGetUniformLocation(shaderProgram, "u_hasMetallicTexture");
		glMaterialData->uniformLocations["u_hasRoughnessTexture"] = glGetUniformLocation(shaderProgram, "u_hasRoughnessTexture");
	}

	std::string OpenGLBackend::injectFeatureDefines(const std::string& source,
		const std::set<MaterialFeature>& features)
	{
		std::stringstream defines;

		// Generate #define statements based on enabled features
		if (features.contains(MaterialFeature::BaseColorTexture)) 
		{
			defines << "#define HAS_BASE_COLOR_TEXTURE\n";
		}
		if (features.contains(MaterialFeature::NormalTexture)) 
		{
			defines << "#define HAS_NORMAL_TEXTURE\n";
		}
		if (features.contains(MaterialFeature::MetallicTexture)) 
		{
			defines << "#define HAS_METALLIC_TEXTURE\n";
		}
		if (features.contains(MaterialFeature::RoughnessTexture)) 
		{
			defines << "#define HAS_ROUGHNESS_TEXTURE\n";
		}
		if (features.contains(MaterialFeature::EmissionTexture)) 
		{
			defines << "#define HAS_EMISSION_TEXTURE\n";
		}
		if (features.contains(MaterialFeature::Transparency)) 
		{
			defines << "#define HAS_TRANSPARENCY\n";
		}
		if (features.contains(MaterialFeature::DoubleSided)) 
		{
			defines << "#define DOUBLE_SIDED\n";
		}

		// Insert defines after #version directive
		if (size_t versionPos = source.find("#version"); versionPos != std::string::npos) 
		{
			size_t lineEnd = source.find('\n', versionPos);
			if (lineEnd != std::string::npos) {
				std::string result = source.substr(0, lineEnd + 1);
				result += defines.str();
				result += source.substr(lineEnd + 1);
				return result;
			}
		}

		// If no #version found, prepend defines
		return defines.str() + source;
	}

	void OpenGLBackend::setUniformFromData(GLuint shaderProgram, const std::string& name, const UniformBufferBase& uniform)
	{
		GLint location = glGetUniformLocation(shaderProgram, name.c_str());
		if (location == -1) return;


		// Handle different uniform types based on the uniform's type info
		switch (uniform.getType()) 
		{
		case UniformType::FLOAT:
		{
			float value;
			uniform.getData(&value, sizeof(float));
			glUniform1f(location, value);
		}
		break;
		case UniformType::VEC2:
		{
			Vec2 value;
			uniform.getData(&value, sizeof(float) * 2);
			glUniform2fv(location, 1, &value[0]);
		}
		break;
		case UniformType::VEC3:
		{
			Vec3 value;
			uniform.getData(&value, sizeof(float) * 3);
			glUniform3fv(location, 1, &value[0]);
		}
		break;
		case UniformType::VEC4:
		{
			Vec4 value;
			uniform.getData(&value, sizeof(float) * 3);
			glUniform4fv(location, 1, &value[0]);
		}
		break;
		case UniformType::MAT4:
		{
			Mat4 value;
			uniform.getData(&value, sizeof(float) * 16);
			glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
		}
		break;
		case UniformType::INT:
		{
			int value;
			uniform.getData(&value, sizeof(int));
			glUniform1i(location, value);
		}
		break;
		case UniformType::BOOL:
		{
			bool value;
			uniform.getData(&value, sizeof(bool));
			glUniform1i(location, value ? 1 : 0);
		}
		break;
		default:
			std::cerr << "Unsupported uniform type for: " << name << std::endl;
			break;
		}

		checkGLError("set uniform: " + name);
	}

	OpenGLMeshData* OpenGLBackend::getOrCreateMeshData(Handle meshHandle) 
	{
		auto found = chai::AssetManager::instance().get<Mesh>(meshHandle, [&](const Mesh& m) {
			auto it = m_meshCache.find(meshHandle.index);
			if (it == m_meshCache.end())
			{
				auto glMeshData = std::make_unique<OpenGLMeshData>();
				auto* ptr = glMeshData.get();
				m_meshCache[meshHandle.index] = std::move(glMeshData);
				return ptr;
			}
			return it->second.get();
			});

		if (!found.has_value())
		{
			std::cerr << "Invalid mesh handle, cannot upload to GPU" << std::endl;
			return nullptr;
		}

		return found.value();
	}

	OpenGLMaterialData* OpenGLBackend::getOrCreateMaterialData(IMaterial* material) 
	{
		auto it = m_materialCache.find(material);
		if (it == m_materialCache.end()) 
		{
			auto glMaterialData = std::make_unique<OpenGLMaterialData>();
			auto* ptr = glMaterialData.get();
			m_materialCache[material] = std::move(glMaterialData);
			return ptr;
		}
		return it->second.get();
	}

	void OpenGLBackend::uploadMeshToGPU(const Handle meshHandle, OpenGLMeshData* glMeshData) 
	{
		if (glMeshData->isUploaded) return;

		chai::AssetManager::instance().get<Mesh>(meshHandle, [&, glMeshData](const Mesh& mesh) {
			const auto& vertices = mesh.getVertices();
			const auto& indices = mesh.getIndices();

			// Generate buffers
			glGenVertexArrays(1, &glMeshData->VAO);
			glGenBuffers(1, &glMeshData->VBO);
			glGenBuffers(1, &glMeshData->EBO);

			// Bind VAO
			glBindVertexArray(glMeshData->VAO);

			// Upload vertices
			glBindBuffer(GL_ARRAY_BUFFER, glMeshData->VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

			// Upload indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMeshData->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

			// Set up vertex attributes
			setupVertexAttributes();

			glMeshData->indexCount = indices.size();
			glMeshData->isUploaded = true;

			// Unbind
			glBindVertexArray(0);
			});
	}

	void OpenGLBackend::setupVertexAttributes() 
	{
		// Assuming Vertex structure: position(3f), normal(3f), texCoord(2f)
		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		// Texture coordinate attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		glEnableVertexAttribArray(2);
	}

	void OpenGLBackend::compileMaterial(IMaterial* material, OpenGLMaterialData* glMaterialData) 
	{
		// Cast to our concrete Material type to access shader description
		auto const* concreteMaterial = static_cast<Material*>(material);
		auto shaderDesc = concreteMaterial->getShaderDescription();

		if (!shaderDesc) 
		{
			std::cerr << "Material has no shader description, using default" << std::endl;
			glMaterialData->shaderProgram = defaultShaderProgram;
			glMaterialData->isCompiled = true;
			return;
		}

		// Generate shader hash including material features for variants
		std::string shaderHash = concreteMaterial->getShaderHash();

		// Check if we already have this shader variant compiled
		if (auto it = m_shaderCache.find(shaderHash); it != m_shaderCache.end()) 
		{
			glMaterialData->shaderProgram = it->second;
			glMaterialData->isCompiled = true;
			cacheUniformLocations(glMaterialData->shaderProgram, glMaterialData);
			return;
		}

		// Compile new shader variant

		if (GLuint shaderProgram = compileShaderFromDescription(shaderDesc, concreteMaterial->getEnabledFeatures()); shaderProgram != 0) 
		{
			m_shaderCache[shaderHash] = shaderProgram;
			glMaterialData->shaderProgram = shaderProgram;
		}
		else 
		{
			std::cerr << "Failed to compile material shader, using default" << std::endl;
			glMaterialData->shaderProgram = defaultShaderProgram;
		}

		glMaterialData->isCompiled = true;
		cacheUniformLocations(glMaterialData->shaderProgram, glMaterialData);

	}

	GLuint OpenGLBackend::compileShaderFromDescription(std::shared_ptr<ShaderDescription> shaderDesc,
		const std::set<MaterialFeature>& features)
	{
		std::string vertexSource, fragmentSource;

		// Load shader files
		for (const auto& stage : shaderDesc->stages) {
			std::string source = loadShaderFile(stage.path);
			if (source.empty()) 
			{
				std::cerr << "Failed to load shader file: " << stage.path << std::endl;
				return 0;
			}

			// Generate feature-based preprocessor defines
			source = injectFeatureDefines(source, features);

			if (stage.type == ShaderStage::Vertex) 
			{
				vertexSource = source;
			}
			else if (stage.type == ShaderStage::Fragment) 
			{
				fragmentSource = source;
			}
		}

		if (vertexSource.empty() || fragmentSource.empty()) 
		{
			std::cerr << "Missing vertex or fragment shader source" << std::endl;
			return 0;
		}

		// Compile shaders
		GLuint vertexShader = compileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
		GLuint fragmentShader = compileShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

		if (vertexShader == 0 || fragmentShader == 0) 
		{
			if (vertexShader != 0) glDeleteShader(vertexShader);
			if (fragmentShader != 0) glDeleteShader(fragmentShader);
			return 0;
		}

		// Link program
		GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		// Check linking
		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) 
		{
			char infoLog[512];
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cerr << "Shader program linking failed (" << shaderDesc->name << "): " << infoLog << std::endl;
			glDeleteProgram(program);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		std::cout << "Compiled shader variant: " << shaderDesc->name << std::endl;
		return program;
	}

	std::string OpenGLBackend::loadShaderFile(const std::string& filePath)
	{
		//TEMP
		auto absolutePath = CMAKE_SOURCE_DIR + filePath;
		std::ifstream file(absolutePath);
		if (!file.is_open()) 
		{
			std::cerr << "Failed to open shader file: " << filePath << std::endl;
			return "";
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void OpenGLBackend::bindShaderProgram(GLuint program) 
	{
		if (currentShaderProgram != program) 
		{
			glUseProgram(program);
			currentShaderProgram = program;
		}
	}

	void OpenGLBackend::bindVertexArray(GLuint vao) 
	{
		if (currentVAO != vao) 
		{
			glBindVertexArray(vao);
			currentVAO = vao;
		}
	}
}