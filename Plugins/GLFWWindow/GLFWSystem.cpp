#include "GLFWSystem.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <Window/Window.h>
#include <Window/WindowManager.h>

namespace chai
{
	void glfwCloseCallback(GLFWwindow* window)
	{
		auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (userPtr)
		{
			userPtr->manager->requestClose(userPtr->window->getId());
		}
	}

	GLFWSystem::GLFWSystem()
	{

	}

	GLFWSystem::~GLFWSystem()
	{
	}

	bool GLFWSystem::initialize()
	{
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW!" << std::endl;
			return false;
		}

		return true;
	}

	void GLFWSystem::shutdown()
	{
		glfwTerminate();
	}

	// Window management
	std::unique_ptr<Window> GLFWSystem::createWindow(const WindowDesc& desc, WindowManager* manager)
	{
		auto window = std::make_unique<Window>(desc);
		auto nativeWindow = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), NULL, NULL);
		if (!nativeWindow)
		{
			std::cerr << "Failed to create GLFW Window!" << std::endl;
		}

		glfwSetWindowCloseCallback(nativeWindow, glfwCloseCallback);

		window->setWindowData(WindowData{window.get(), manager});
		window->setNativeWindow(nativeWindow);
		glfwSetWindowUserPointer(nativeWindow, &window->getWindowData());

		//is this opengl specific?
		glfwMakeContextCurrent(nativeWindow);

		return std::move(window);
	}

	void GLFWSystem::pollEvents()
	{
		glfwPollEvents();
	}

	GLFWwindow* GLFWSystem::convertToGLFWWindow(void* nativeWindow)
	{
		return static_cast<GLFWwindow*>(nativeWindow);
	}

	void GLFWSystem::destroyWindow(void* nativeWindow)
	{
		if (auto glfwWindow = convertToGLFWWindow(nativeWindow); glfwWindow)
		{
			glfwDestroyWindow(glfwWindow);
		}
	}

	void* GLFWSystem::getProcAddress()
	{
		return glfwGetProcAddress;
	}

	void GLFWSystem::swapBuffers(void* nativeWindow)
	{
		glfwSwapBuffers(convertToGLFWWindow(nativeWindow));
	}

	void GLFWSystem::destroyAllWindows() {}
}