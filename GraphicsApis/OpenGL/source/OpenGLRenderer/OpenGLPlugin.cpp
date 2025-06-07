//#pragma once
//#include <OpenGLRenderer/OpenGLPlugin.h>
//#include <OpenGLRenderer/OpenGLMesh.h>
//#include <OpenGLRenderer/OpenGLMaterial.h>
//
//namespace chai::brew
//{
//	void checkGLError(const std::string& context) 
//	{
//		GLenum err;
//		while ((err = glGetError()) != GL_NO_ERROR) {
//			std::cerr << "OpenGL error in " << context << ": " << err << std::endl;
//		}
//	}
//
//	bool OpenGLPlugin::initialize(void* procAddress) 
//	{
//		if (m_initialized) return true;
//
//		if (!gladLoadGL((GLADloadfunc)procAddress)) {
//			std::cerr << "Failed to initialize GLAD" << std::endl;
//			return false;
//		}
//		checkGLError("setProcAddress");
//		glEnable(GL_CULL_FACE);
//
//		// Set up default OpenGL state
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_CULL_FACE);
//
//		m_initialized = true;
//		return true;
//	}
//
//	void OpenGLPlugin::OnShutdown() 
//	{
//		// Cleanup OpenGL resources
//		m_initialized = false;
//	}
//
//	std::unique_ptr<IMesh> OpenGLPlugin::createMesh()
//	{
//		return std::make_unique<OpenGLMesh>();
//	}
//
//	std::unique_ptr<IMaterial> OpenGLPlugin::createMaterial()
//	{
//		return std::make_unique<OpenGLMaterial>();
//	}
//
//	std::unique_ptr<IMesh> OpenGLPlugin::createTriangle()
//	{
//		auto mesh = createMesh();
//		if (!mesh) return nullptr;
//
//		std::vector<Vertex> vertices = {
//			Vertex(0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f),
//			Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f),
//			Vertex(0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f)
//		};
//
//		mesh->setVertices(vertices);
//		return mesh;
//	}
//
//	std::unique_ptr<IMesh> OpenGLPlugin::createQuad()
//	{
//		auto mesh = createMesh();
//		if (!mesh) return nullptr;
//
//		std::vector<Vertex> vertices = {
//			Vertex(-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f),
//			Vertex(0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),
//			Vertex(0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f),
//			Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f)
//		};
//
//		std::vector<unsigned int> indices = { 0, 1, 2,  2, 3, 0 };
//
//		mesh->setVertices(vertices);
//		mesh->setIndices(indices);
//		return mesh;
//	}
//
//	std::unique_ptr<IMesh> OpenGLPlugin::createCube()
//	{
//		// Implementation for cube mesh...
//		return createMesh(); // Placeholder
//	}
//}