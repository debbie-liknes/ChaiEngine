#include <Meta/ChaiMacros.h>
#include <OpenGLRenderer/OpenGLRenderer.h>
#include <OpenGLRenderer/GLShader.h>

void registerServices()
{
	chai::kettle::PluginRegistry::Instance().Register("Renderer", "OpenGL", [] {
		return static_cast<void*>(new chai::brew::OpenGLBackend());
		});

	chai::kettle::PluginRegistry::Instance().Register("ShaderResourceLoader", "OpenGL", [] {
		return static_cast<void*>(new chai::brew::GLShaderLoader());
		});
}

CHAI_PLUGIN(OpenGL, "1.0.0", "chai::brew", "Renderer", registerServices)