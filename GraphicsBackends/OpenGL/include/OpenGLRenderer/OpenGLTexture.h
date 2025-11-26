#pragma once
#include <Graphics/TextureAsset.h>
#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <Types/CMap.h>
#include <Resource/ResourceManager.h>
#include <memory>
#include <iostream>

namespace chai::brew
{
    /**
     * OpenGL-specific texture data.
     * Stores GPU texture handle and upload state.
     */
    struct OPENGLRENDERER_EXPORT OpenGLTextureData
    {
        GLuint texture = 0;              ///< OpenGL texture handle
        GLenum target = GL_TEXTURE_2D;   ///< Texture target (2D, Cube, etc.)
        GLint internalFormat = GL_RGBA8; ///< Internal GPU format
        GLenum format = GL_RGBA;         ///< Source data format
        GLenum type = GL_UNSIGNED_BYTE;  ///< Source data type

        uint32_t width = 0;     ///< Texture width in pixels
        uint32_t height = 0;    ///< Texture height in pixels
        uint32_t mipLevels = 1; ///< Number of mipmap levels

        bool isUploaded = false; ///< True when GPU upload complete
        bool hasMipmaps = false; ///< True if mipmaps generated
    };

    /**
     * Manages OpenGL texture resources.
     *
     * Responsibilities:
     * - Create and cache texture data structures
     * - Upload texture data to GPU (via UploadQueue)
     * - Generate mipmaps
     * - Configure texture parameters (filtering, wrapping)
     * - Clean up GPU resources on destruction
     *
     * Thread safety: Not thread-safe. Use from render thread only.
     */
    class OPENGLRENDERER_EXPORT OpenGLTextureManager
    {
    public:
        OpenGLTextureManager() = default;
        virtual ~OpenGLTextureManager();

        OpenGLTextureManager(const OpenGLTextureManager&) = delete;
        OpenGLTextureManager& operator=(const OpenGLTextureManager&) = delete;

        OpenGLTextureManager(OpenGLTextureManager&&) = default;
        OpenGLTextureManager& operator=(OpenGLTextureManager&&) = default;

        /**
         * Get or create texture data for the given handle.
         * Creates a new entry if it doesn't exist.
         *
         * @param texHandle Texture resource handle
         * @return Pointer to texture data (never null)
         */
        OpenGLTextureData* getOrCreateTextureData(ResourceHandle texHandle);

        /**
         * Upload texture data to GPU.
         * Called from UploadQueue during texture upload.
         *
         * @param texHandle Texture resource handle
         * @param texData OpenGL texture data to populate
         * @return True if upload successful
         */
        bool uploadTexture(ResourceHandle texHandle, OpenGLTextureData* texData);

    private:
        /**
         * Determine OpenGL format from resource data.
         * Maps source data (RGB, RGBA, etc.) to OpenGL enums.
         */
        void determineFormats(const TextureResource* texResource,
                              OpenGLTextureData* texData);

        /**
         * Configure texture parameters (filtering, wrapping).
         * Sets up sampler state based on texture type and quality settings.
         */
        void configureTextureParameters(OpenGLTextureData* texData,
                                        const TextureResource* texResource);

        CMap<size_t, std::unique_ptr<OpenGLTextureData>> m_textureCache;
    };
} // namespace chai::brew