#pragma once
#include <glad/gl.h>
#include <string>
#include <iostream>
#include <Graphics/VertexAttribute.h>

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

    static GLenum toGLType(const DataType type)
    {
        switch (type)
        {
        case DataType::Float:
        case DataType::Float2:
        case DataType::Float3:
        case DataType::Float4:
            return GL_FLOAT;
        case DataType::Int:
        case DataType::Int2:
        case DataType::Int3:
        case DataType::Int4:
            return GL_INT;
        case DataType::UnsignedByte:
        case DataType::UnsignedByte4:
            return GL_UNSIGNED_BYTE;
        default:
            return GL_FLOAT;
        }
    }
}