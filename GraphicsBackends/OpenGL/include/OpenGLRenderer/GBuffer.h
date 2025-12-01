#pragma once
#include <glad/gl.h>


namespace chai::brew
{
    struct GBuffer {
        GLuint framebuffer;
        GLuint positionTex; // RGB16F - world position
        GLuint normalTex;   // RGB16F - world normal
        GLuint albedoTex;   // RGBA8 - albedo + AO
        GLuint materialTex; // RG8 - metallic, roughness
        GLuint depthTex;    // DEPTH24_STENCIL8
    };

    static void createGBuffer(GBuffer& gb, int width, int height)
    {
        glGenFramebuffers(1, &gb.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gb.framebuffer);

        // Position
        glGenTextures(1, &gb.positionTex);
        glBindTexture(GL_TEXTURE_2D, gb.positionTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb.positionTex, 0);

        // Normal
        glGenTextures(1, &gb.normalTex);
        glBindTexture(GL_TEXTURE_2D, gb.normalTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gb.normalTex, 0);

        // Albedo
        glGenTextures(1, &gb.albedoTex);
        glBindTexture(GL_TEXTURE_2D, gb.albedoTex);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gb.albedoTex, 0);

        // Material (metallic, roughness)
        glGenTextures(1, &gb.materialTex);
        glBindTexture(GL_TEXTURE_2D, gb.materialTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gb.materialTex, 0);

        // Depth
        glGenTextures(1, &gb.depthTex);
        glBindTexture(GL_TEXTURE_2D, gb.depthTex);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_DEPTH24_STENCIL8,
                     width,
                     height,
                     0,
                     GL_DEPTH_STENCIL,
                     GL_UNSIGNED_INT_24_8,
                     nullptr);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gb.depthTex, 0);

        GLenum attachments[] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(4, attachments);
    }
}