#pragma once
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/Vertex.h>
#include <glad/gl.h>
#include <vector>

namespace chai::brew
{
    struct OpenGLMeshData 
    {
        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;
        size_t indexCount = 0;
        bool isUploaded = false;

        ~OpenGLMeshData() 
        {
            if (VAO) glDeleteVertexArrays(1, &VAO);
            if (VBO) glDeleteBuffers(1, &VBO);
            if (EBO) glDeleteBuffers(1, &EBO);
        }
    };
}