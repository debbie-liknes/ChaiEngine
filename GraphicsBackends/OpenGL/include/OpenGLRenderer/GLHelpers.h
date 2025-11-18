#pragma once
#include <glad/gl.h>
#include <string>
#include <iostream>

namespace chai::brew
{
	inline const char* getGLErrorString(GLenum err)
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

	inline static void checkGLError(const std::string& context)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (" << context << "): " << getGLErrorString(err) << std::endl;
		}
	}
}