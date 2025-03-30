#pragma once
#include <set>
#include <memory>
#include <string>
#include <ChaiEngine/UniformBuffer.h>
#include <unordered_map>

namespace chai::brew
{
	struct UniformBindingPair
	{
		std::shared_ptr<UniformBufferBase> ubo;
		unsigned int binding;
	};

	class GLShaderProgram {
	public:
		GLShaderProgram();

		void AddShader(int handle);
		std::set<int> getShaders();

		int getProgramHandle() { return m_programHandle; }

		void addUniform(std::string name, std::shared_ptr<UniformBufferBase> ubo, unsigned int binding);
		void upload(std::string name);
		std::shared_ptr<UniformBufferBase> getUniform(std::string name)
		{
			return m_uniforms[name].ubo;
		}

		void link();
		bool isLinked() const { return m_linked; }
		void use();
		int getNumUniforms() { return m_uniforms.size(); }

	private:
		bool m_linked = false;
		std::set<int> m_shaders;
		int m_programHandle;
		std::unordered_map<std::string, UniformBindingPair> m_uniforms;
	};
}