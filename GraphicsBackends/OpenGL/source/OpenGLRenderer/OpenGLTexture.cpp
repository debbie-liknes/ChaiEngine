#include <OpenGLRenderer/OpenGLTexture.h>
#include <OpenGLRenderer/GLHelpers.h>
#include <Graphics/TextureAsset.h>

namespace chai::brew
{
    OpenGLTextureManager::~OpenGLTextureManager()
    {
        // Clean up all cached texture resources
        for (auto& [id, texData] : m_textureCache) {
            if (texData->texture != 0) {
                glDeleteTextures(1, &texData->texture);
            }
        }
    }

    OpenGLTextureData* OpenGLTextureManager::getOrCreateTextureData(ResourceHandle texHandle)
    {
        auto it = m_textureCache.find(texHandle.index);
        if (it == m_textureCache.end()) {
            auto glTexData = std::make_unique<OpenGLTextureData>();
            auto* ptr = glTexData.get();
            m_textureCache[texHandle.index] = std::move(glTexData);
            return ptr;
        }
        return it->second.get();
    }

    bool OpenGLTextureManager::uploadTexture(ResourceHandle texHandle, OpenGLTextureData* texData)
    {
        if (!texData) {
            std::cerr << "OpenGLTextureManager::uploadTexture: null texture data" << std::endl;
            return false;
        }

        auto* texResource = ResourceManager::instance().getResource<TextureResource>(
            texHandle);
        if (!texResource) {
            std::cerr << "OpenGLTextureManager::uploadTexture: invalid texture handle" << std::endl;
            return false;
        }

        if (texResource->getType() == TextureType::TexCube) {
            texData->target = GL_TEXTURE_CUBE_MAP;
        } else {
            texData->target = GL_TEXTURE_2D;
        }

        // Determine formats from resource
        determineFormats(texResource, texData);

        // Generate texture handle
        if (texData->texture == 0) {
            glGenTextures(1, &texData->texture);
            checkGLError("glGenTextures");
        }

        // Bind texture
        glBindTexture(texData->target, texData->texture);
        checkGLError("glBindTexture");

        if (texResource->getType() == TextureType::Tex2D) {
            // Store dimensions
            texData->width = texResource->getWidth();
            texData->height = texResource->getHeight();

            // Upload texture data
            glTexImage2D(
                texData->target,
                0,
                // Mip level 0 (base)
                texData->internalFormat,
                // GPU internal format
                texData->width,
                texData->height,
                0,
                // Border (must be 0)
                texData->format,
                // Source data format
                texData->type,
                // Source data type
                texResource->getPixels() // Pixel data
                );
            checkGLError("glTexImage2D");
        }
        else if (texResource->getType() == TextureType::TexCube) {
            texData->width = texResource->getWidth();
            texData->height = texResource->getHeight();

            const size_t faceCount = texResource->getFaceCount();
            if (faceCount != 6) {
                std::cerr << "Cubemap texture must have 6 faces, got " << faceCount << "\n";
                return false;
            }

            for (int i = 0; i < 6; i++) {
                const auto& face = texResource->getFace(i);

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    texData->internalFormat,
                    face.width,
                    face.height,
                    0,
                    texData->format,
                    texData->type,
                    face.pixels.data()
                );
                checkGLError("glTexImage2D (cube face)");
            }
        }

        // Generate mipmaps if requested
        if (texResource->generateMipmaps()) {
            glGenerateMipmap(texData->target);
            checkGLError("glGenerateMipmap");
            texData->hasMipmaps = true;

            // Calculate mip levels
            texData->mipLevels = 1 + static_cast<uint32_t>(
                                     std::floor(
                                         std::log2(std::max(texData->width, texData->height)))
                                 );
        }

        // Configure texture parameters
        configureTextureParameters(texData, texResource);

        // Unbind
        glBindTexture(texData->target, 0);

        texData->isUploaded = true;

        std::cout << "Uploaded texture: " << texData->width << "x" << texData->height
            << " (" << (texData->hasMipmaps ? "with mipmaps" : "no mipmaps") << ")"
            << std::endl;

        return true;
    }

    void OpenGLTextureManager::determineFormats(const TextureResource* texResource,
                                                OpenGLTextureData* texData)
    {
        // Determine format based on channel count
        switch (texResource->getChannels()) {
            case 1: // Grayscale
                texData->format = GL_RED;
                texData->internalFormat = GL_R8;
                break;
            case 2: // Grayscale + Alpha
                texData->format = GL_RG;
                texData->internalFormat = GL_RG8;
                break;
            case 3: // RGB
                texData->format = GL_RGB;
                texData->internalFormat = texResource->getColorSpace() == ColorSpace::SRGB ? GL_SRGB8 : GL_RGB8;
                break;
            case 4: // RGBA
                texData->format = GL_RGBA;
                texData->internalFormat = texResource->getColorSpace() == ColorSpace::SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                break;
            default:
                std::cerr << "Unsupported channel count: " << texResource->getChannels() << std::endl;
                texData->format = GL_RGBA;
                texData->internalFormat = GL_RGBA8;
        }

        // Data type (could be extended for HDR, etc.)
        texData->type = GL_UNSIGNED_BYTE;
    }

    void OpenGLTextureManager::configureTextureParameters(OpenGLTextureData* texData,
                                                          const TextureResource* texResource)
    {
        GLenum target = texData->target;

        // Filtering
        if (texData->hasMipmaps) {
            // Trilinear filtering (best quality with mipmaps)
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            // Bilinear filtering (no mipmaps)
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        checkGLError("texture filtering");

        // Wrapping mode
        GLenum wrapS = GL_REPEAT;
        GLenum wrapT = GL_REPEAT;
        GLenum wrapR = GL_REPEAT;

        if (target == GL_TEXTURE_CUBE_MAP) {
            // Cubemaps should basically always clamp
            wrapS = GL_CLAMP_TO_EDGE;
            wrapT = GL_CLAMP_TO_EDGE;
            wrapR = GL_CLAMP_TO_EDGE;
        }
        else {
            // Map from resource wrap mode to OpenGL
            if (texResource->getWrapMode() == TextureWrapMode::Clamp) {
                wrapS = wrapT = GL_CLAMP_TO_EDGE;
            } else if (texResource->getWrapMode() == TextureWrapMode::Mirror) {
                wrapS = wrapT = GL_MIRRORED_REPEAT;
            } else if (texResource->getWrapMode() == TextureWrapMode::Repeat) {
                wrapS = wrapT = GL_REPEAT;
            }
        }


        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
        if (target == GL_TEXTURE_CUBE_MAP) {
            glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
        }
        checkGLError("texture wrapping");

        // Anisotropic filtering (if supported and requested)
        /*if (texResource->useAnisotropicFiltering) {
            GLfloat maxAnisotropy = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            if (maxAnisotropy > 1.0f) {
                glTexParameterf(target,
                                GL_TEXTURE_MAX_ANISOTROPY_EXT,
                                std::min(16.0f, maxAnisotropy));
                checkGLError("anisotropic filtering");
            }
        }*/
    }
} // namespace chai::brew