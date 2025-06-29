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

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (userPtr)
		{
			if(action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				InputSystem::instance().queueEvent(std::make_unique<KeyPressEvent>(userPtr->window->getId(), key));
			}
			else if(action == GLFW_RELEASE)
			{
				InputSystem::instance().queueEvent(std::make_unique<KeyReleaseEvent>(userPtr->window->getId(), key));
			}
		}
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (userPtr)
		{
			if (action == GLFW_PRESS)
			{
				InputSystem::instance().queueEvent(std::make_unique<MouseDownEvent>(userPtr->window->getId(), (uint32_t)button));
			}
			else if (action == GLFW_RELEASE)
			{
				InputSystem::instance().queueEvent(std::make_unique<MouseUpEvent>(userPtr->window->getId(), (uint32_t)button));
			}
		}
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (userPtr)
		{
			InputSystem::instance().queueEvent(std::make_unique<MouseMoveEvent>(userPtr->window->getId(), xpos, ypos));
		}
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (userPtr)
		{
			InputSystem::instance().queueEvent(std::make_unique<FrameBufferResize>(userPtr->window->getId(), width, height));
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
		glfwSetKeyCallback(nativeWindow, key_callback);
		glfwSetMouseButtonCallback(nativeWindow, mouse_button_callback);
		glfwSetCursorPosCallback(nativeWindow, cursor_position_callback);
		glfwSetFramebufferSizeCallback(nativeWindow, framebuffer_size_callback);

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