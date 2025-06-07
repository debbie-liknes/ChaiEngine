#pragma once
#include <Meta/ChaiMacros.h>
#include <glad/gl.h>
#include <iostream>

namespace chai::brew
{
	//class OpenGLPlugin : public IGraphicsPlugin 
	//{
	//public:
	//	virtual const std::string GetName() const { return "OpenGL"; }
	//	virtual void OnStartup() {}
	//	virtual void OnShutdown();

	//	bool initialize(void* procAddress) override;
	//	bool isInitialized() const override { return m_initialized; }

	//	std::unique_ptr<IMesh> createMesh() override;
	//	std::unique_ptr<IMaterial> createMaterial() override;
	//	std::unique_ptr<IMesh> createTriangle() override;
	//	std::unique_ptr<IMesh> createQuad() override;
	//	std::unique_ptr<IMesh> createCube() override;

	//private:
	//	bool m_initialized = false;
	//};
}

//CHAI_PLUGIN_CLASS(OpenGLPlugin) {
//	//CHAI_SERVICE(WindowGLFW, "window");
//	//CHAI_SERVICE(InputGLFW, "input");
//	//CHAI_SERVICE_FACTORY(RendererGLFW, "renderer", []() {
//	//	auto r = std::make_shared<RendererGLFW>();
//	//	r->Initialize();
//	//	return r;
//	//	});
//}
//CHAI_REGISTER_PLUGIN(OpenGLPlugin, "OpenGLPlugin", "1.0.0")