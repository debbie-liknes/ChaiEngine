#include <OpenGLRenderer/OpenGLRenderer.h>
#include <algorithm>
#include <chrono>
#include <OpenGLRenderer/GLHelpers.h>
#include <OpenGLRenderer/GLPipeline.h>

namespace chai::brew
{
    bool OpenGLBackend::initialize(void* winProcAddress)
    {
        if (!gladLoadGL((GLADloadfunc)winProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            checkGLError("setProcAddress");
            return false;
        }

        checkGLError("setProcAddress");

        // Check OpenGL version
        const auto* version = (const char*)glGetString(GL_VERSION);
        std::cout << "OpenGL Version: " << version << std::endl;

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);  // or GL_LEQUAL

        // Enable back-face culling (optional but recommended)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);  // Counter-clockwise is front

        // Disable blending by default (we enable it only for transparent pass)
        glDisable(GL_BLEND);

        m_matManager.setShaderManager(&m_shaderManager);

        std::cout << "Creating common uniforms..." << std::endl;

        m_perFrameUBOData = createUniform<CommonUniforms>();
        m_perFrameUBOData->setValue({ Mat4::identity() , Mat4::identity() });

        m_perDrawUBOData = createUniform<DrawUniforms>();
        m_perDrawUBOData->setValue(DrawUniforms{ Mat4::identity() , Mat4::identity() });

        m_uniManager.buildUniforms({ m_perFrameUBOData.get(), m_perDrawUBOData.get() });

        // Create default shader
        defaultShaderProgram = m_shaderManager.createDefaultShaderProgram();
        if (defaultShaderProgram == 0)
        {
            std::cerr << "Failed to create default shader program" << std::endl;
            return false;
        }

        std::cout << "OpenGL Backend initialized successfully" << std::endl;
        return true;
    }

    void OpenGLBackend::shutdown()
    {
        std::cout << "Shutting down OpenGL Backend..." << std::endl;

        // Delete default shader
        if (defaultShaderProgram != 0) {
            glDeleteProgram(defaultShaderProgram);
            defaultShaderProgram = 0;
        }

        std::cout << "OpenGL Backend shutdown complete" << std::endl;
    }

    // ============================================================================
    // OPTIMIZED COMMAND EXECUTION
    // ============================================================================

    void OpenGLBackend::executeCommands(const std::vector<RenderCommand>& commands)
    {
        if (commands.empty()) return;

        // Separate commands by type
        std::vector<SortedDrawCommand> opaqueDraws;
        std::vector<SortedDrawCommand> transparentDraws;

        // Process Uploads
        for (const auto& cmd : commands)
        {
            switch (cmd.type)
            {
            case RenderCommand::SET_VIEWPORT:
            {
                int x, y, width, height;
                cmd.viewport->getViewport(x, y, width, height);
                glViewport(x, y, width, height);
                m_perFrameUBOData->setValue({ cmd.viewMatrix, cmd.projectionMatrix });

            }
            break;

            case RenderCommand::CLEAR:
                // Clear before sorting/drawing
                clear(0.0f, 0.0f, 0.0f, 1.0f);
                break;

            case RenderCommand::SET_LIGHTS:
                m_cachedLights = cmd.lights;
                m_lightsDirty = true;
                break;

            case RenderCommand::DRAW_MESH:
            {
                if (!cmd.mesh.isValid()) continue;

                // Get or create mesh data
                OpenGLMeshData* meshData = m_meshManager.getOrCreateMeshData(cmd.mesh);
                if (!meshData) continue;

                //Schedule upload if needed (non-blocking)
                if (!meshData->isUploaded)
                {
                    if (!m_uploadQueue.isQueued(cmd.mesh))
                    {
                        m_uploadQueue.requestUpload(cmd.mesh, (void*)meshData);
                    }
                    continue;  // Skip this frame, will render when ready
                }

                //Create sort key
                SortedDrawCommand sorted;
                sorted.command = cmd;
                sorted.sortKey = createSortKey(cmd, meshData);
                
                // Categorize by transparency
                if (sorted.sortKey.transparency)
                {
                    transparentDraws.push_back(sorted);
                }
                else 
                {
                    opaqueDraws.push_back(sorted);
                }
            }
            break;
            }
        }

        // Sort opaque front-to-back (minimize overdraw, maximize early-Z)
        // Sort by: shader -> material -> mesh -> depth
        std::sort(opaqueDraws.begin(), opaqueDraws.end(),
            [](const SortedDrawCommand& a, const SortedDrawCommand& b) {
                return a.sortKey < b.sortKey;
            });

        // Sort transparent back-to-front (correct alpha blending)
        // Depth is most significant for transparents
        std::sort(transparentDraws.begin(), transparentDraws.end(),
            [](const SortedDrawCommand& a, const SortedDrawCommand& b) {
                return a.sortKey.depth > b.sortKey.depth;
            });

        // Update per-frame uniforms once
        updatePerFrameUniforms();

        // Execute render passes
        if (!opaqueDraws.empty()) {
            // Opaque pass
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            drawBatchedCommands(opaqueDraws);
        }

        if (!transparentDraws.empty()) {
            // Transparent pass
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);  // Don't write to depth buffer
            drawBatchedCommands(transparentDraws);
            glDepthMask(GL_TRUE);
        }
    }

    // ============================================================================
    // COMMAND EXECUTION
    // ============================================================================

    void OpenGLBackend::clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // ============================================================================
    // SORT KEY CREATION
    // ============================================================================

    RenderKey OpenGLBackend::createSortKey(const RenderCommand& cmd,
        OpenGLMeshData* meshData)
    {
        RenderKey key;

        // Determine transparency
        bool isTransparent = false;
        OpenGLMaterialData* matData = nullptr;

        if (cmd.material.isValid()) 
        {
            matData = m_matManager.getOrCreateMaterialData(cmd.material);
            if (matData && matData->isCompiled) 
            {
                // Check material properties for transparency
                isTransparent = matData->isTransparent;
            }
        }

        key.transparency = isTransparent ? 1 : 0;

        // Calculate depth from camera (assumes cmd has view matrix access)
        // This is simplified - you'd use actual camera position
        Vec3 position = Vec3(cmd.transform[3][0], cmd.transform[3][1], cmd.transform[3][2]);
        float depth = length(position - m_cameraPosition);

        // Quantize depth to 23 bits (0 to ~8 million units)
        uint32_t quantizedDepth = static_cast<uint32_t>(
            std::clamp(depth * 1000.0f, 0.0f, 8388607.0f));
        key.depth = quantizedDepth;

        // Shader ID (for batching)
        GLuint shaderProgram = defaultShaderProgram;
        if (matData && matData->shaderProgram) 
        {
            shaderProgram = matData->shaderProgram;
        }
        // Use upper bits of program ID for better distribution
        key.shader = (shaderProgram >> 4) & 0xFFFF;

        // Material ID (for batching)
        key.material = cmd.material.isValid() ?
            (cmd.material.index & 0xFFF) : 0;

        // Mesh ID (for batching)
        key.mesh = (meshData->VAO >> 4) & 0xFFF;

        return key;
    }

    // ============================================================================
    // BATCHED DRAWING (State Change Minimization)
    // ============================================================================

    void OpenGLBackend::updateShader(OpenGLShaderData* shaderData, uint32_t& stateChanges)
    {
        glUseProgram(shaderData->program);
        stateChanges++;

        //apply pipeline description?

        auto uni = m_uniManager.getUniformBufferData(*m_perFrameUBOData);

        if (shaderData && uni)
        {
            // Re-bind per-frame UBO (shader changed)
            glBindBufferBase(GL_UNIFORM_BUFFER, shaderData->perFrameUBOBinding, uni->ubo);

            // Update lights if dirty
            if (m_lightsDirty)
            {
                setLightsUniforms(shaderData);
            }
        }
    }

    void OpenGLBackend::drawBatchedCommands(const std::vector<SortedDrawCommand>& sortedDraws)
    {
        if (sortedDraws.empty()) return;

        GLuint currentShader = 0;
        GLuint currentVAO = 0;
        int currentMaterialID = -1;

		uint32_t stateChanges = 0;
		uint32_t drawCalls = 0;

        for (const auto& sorted : sortedDraws)
        {
            const RenderCommand& cmd = sorted.command;
            OpenGLMeshData* meshData = m_meshManager.getOrCreateMeshData(cmd.mesh);
            if (!meshData || !meshData->isUploaded) continue;

            // Get material data
            OpenGLMaterialData* matData = cmd.material.isValid() ?
                m_matManager.getOrCreateMaterialData(cmd.material) : nullptr;

            // COMPILE MATERIAL IF NEEDED (NEW)
            if (matData && !matData->isCompiled)
            {
                m_matManager.compileMaterial(cmd.material, matData);
            }

            // Get shader (either from material or default)
            OpenGLShaderData* shaderData = m_shaderManager.getShaderData(
                (matData && matData->shaderProgram) ?
                matData->shaderProgram : defaultShaderProgram);

            // Change shader only when necessary
            if (shaderData && shaderData->program != currentShader)
            {
                currentShader = shaderData->program;
                updateShader(shaderData, stateChanges);
            }

            // Change VAO only when necessary
            if (meshData->VAO != currentVAO)
            {
                glBindVertexArray(meshData->VAO);
                currentVAO = meshData->VAO;
                stateChanges++;
            }

            // Apply material properties if material changed
            if (cmd.material.index != currentMaterialID)
            {
                if (cmd.material.isValid() && matData && matData->isCompiled)
                {
                    applyMaterialState(matData, shaderData);
                }
                currentMaterialID = cmd.material.index;  // ALWAYS update tracking
            }

            // Set per-draw uniforms
            updatePerDrawUniforms(cmd, shaderData);

            // DRAW!
            glDrawElements(GL_TRIANGLES, meshData->indexCount, GL_UNSIGNED_INT, 0);
            drawCalls++;
        }
    }

    // ============================================================================
    // UNIFORM BUFFER OBJECTS (Fast Uniform Updates)
    // ============================================================================

    void OpenGLBackend::updatePerFrameUniforms()
    {
        // Update per-frame UBO once per frame
		m_uniManager.updateUniform(*m_perFrameUBOData);
    }

    void OpenGLBackend::setLightsUniforms(OpenGLShaderData* shaderData)
    {
        //if (shaderData->u_lightCount != -1)
        //{
        //    glUniform1i(shaderData->u_lightCount, (GLint)m_cachedLights.size());
        //}

        //for (size_t i = 0; i < m_cachedLights.size() && i < 16; ++i)
        //{
        //    const auto& lightLocs = shaderData->lights[i];
        //    const Light& light = m_cachedLights[i];

        //    // Direct uniform calls - no string lookups!
        //    if (lightLocs.type != -1) glUniform1i(lightLocs.type, light.type);
        //    if (lightLocs.position != -1) glUniform3fv(lightLocs.position, 1, &light.position[0]);
        //    if (lightLocs.direction != -1) glUniform3fv(lightLocs.direction, 1, &light.direction[0]);
        //    if (lightLocs.color != -1) glUniform3fv(lightLocs.color, 1, &light.color[0]);
        //    if (lightLocs.intensity != -1) glUniform1f(lightLocs.intensity, light.intensity);
        //    if (lightLocs.range != -1) glUniform1f(lightLocs.range, light.range);
        //    if (lightLocs.attenuation != -1) glUniform3fv(lightLocs.attenuation, 1, &light.attenuation[0]);
        //    if (lightLocs.innerCone != -1) glUniform1f(lightLocs.innerCone, light.innerCone);
        //    if (lightLocs.outerCone != -1) glUniform1f(lightLocs.outerCone, light.outerCone);
        //    if (lightLocs.enabled != -1) glUniform1i(lightLocs.enabled, light.enabled);
        //}
    }

    void OpenGLBackend::updatePerDrawUniforms(const RenderCommand& cmd,
        OpenGLShaderData* shaderData)
    {
        if (!shaderData) return;

        DrawUniforms uniforms{};
        uniforms.model = cmd.transform;

        Mat3 normalMat = toMat3(cmd.transform).inverse().transpose();
        uniforms.normalMatrix = toMat4(normalMat);

        // Update CPU→GPU
        m_perDrawUBOData->setValue(uniforms);
        m_uniManager.updateUniform(*m_perDrawUBOData);

        // Bind to the per-draw binding index for this program
        if (auto ub = m_uniManager.getUniformBufferData(*m_perDrawUBOData))
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, shaderData->perDrawUBOBinding, ub->ubo);
        }
    }

    void OpenGLBackend::setUniformValue(GLint location, const std::unique_ptr<UniformBufferBase>& uniform)
    {
        if (location == -1 || !uniform) return;
        switch (uniform->getType())
        {
        case UniformType::FLOAT:
        {
            float value;
            uniform->getData(&value, sizeof(float));
            glUniform1f(location, value);
        }
        break;
        case UniformType::VEC2:
        {
            Vec2 value;
            uniform->getData(&value, sizeof(Vec2));
            glUniform2fv(location, 1, &value[0]);
        }
        break;
        case UniformType::VEC3:
        {
            Vec3 value;
            uniform->getData(&value, sizeof(Vec3));
            glUniform3fv(location, 1, &value[0]);
        }
        break;
        case UniformType::VEC4:
        {
            Vec4 value;
            uniform->getData(&value, sizeof(Vec4));
            glUniform4fv(location, 1, &value[0]);
        }
        break;
        case UniformType::MAT4:
        {
            Mat4 value;
            uniform->getData(&value, sizeof(Mat4));
            glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        }
        break;
        case UniformType::INT:
        {
            int value;
            uniform->getData(&value, sizeof(int));
            glUniform1i(location, value);
        }
        break;
        case UniformType::BOOL:
        {
            bool value;
            uniform->getData(&value, sizeof(bool));
            glUniform1i(location, value ? 1 : 0);
        }
        break;
        default:
            break;
		}
    }

    // ============================================================================
    // MATERIAL STATE APPLICATION
    // ============================================================================

    void OpenGLBackend::applyMaterialState(OpenGLMaterialData* matData,
        OpenGLShaderData* shaderData)
    {
        if (!matData || !shaderData) return;

        // Bind textures later

        // Set material uniforms (color, roughness, metallic, etc.)
        for (const auto& [name, uniform] : matData->uniforms) 
        {
            auto it = shaderData->uniformLocations.find(name);
            if (it != shaderData->uniformLocations.end()) 
            {
                setUniformValue(it->second, uniform);
            }
        }
    }

    // ============================================================================
    // ASYNC UPLOAD SYSTEM
    // ============================================================================

    void OpenGLBackend::beginFrame()
    {
        // Process pending uploads with time budget (2ms)
        m_uploadQueue.processUploads(2.0f);

        // Reset per-frame stats
        //m_stats.reset();
    }
}