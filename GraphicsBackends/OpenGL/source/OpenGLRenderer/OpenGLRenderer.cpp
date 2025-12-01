#include <OpenGLRenderer/OpenGLRenderer.h>
#include <algorithm>
#include <chrono>
#include <OpenGLRenderer/GLHelpers.h>

#include "Graphics/ShaderAsset.h"

namespace chai::brew
{
    OpenGLBackend::OpenGLBackend(){}
    OpenGLBackend::~OpenGLBackend() {}

    bool OpenGLBackend::initialize(std::unique_ptr<RenderSurface> surface, void* winProcAddress)
    {
        m_winProcAddress = winProcAddress;
        m_surface = std::move(surface);

        // Start the render thread
        if (!startRenderThread()) {
            std::cerr << "Failed to start render thread" << std::endl;
            return false;
        }

        // Wait for render thread to complete initialization
        while (!m_initialized.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::cout << "OpenGL Backend initialized successfully (threaded)" << std::endl;
        return true;
    }

    void OpenGLBackend::shutdown()
    {
        waitForIdle();

        stopRenderThread();
    }

    // ============================================================================
    // RENDER THREAD MANAGEMENT
    // ============================================================================
    bool OpenGLBackend::startRenderThread()
    {
        if (m_running.load()) {
            std::cerr << "Render thread already running" << std::endl;
            return false;
        }

        m_running.store(true);
        m_renderThread = std::thread(&OpenGLBackend::renderThreadMain, this);

        return true;
    }

    void OpenGLBackend::stopRenderThread()
    {
        if (!m_running.load())
            return;

        m_running.store(false);
        m_queueCV.notify_one();

        // Wait for thread to signal it's done
        while (!m_threadExited.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Now safe to detach
        if (m_renderThread.joinable()) {
            m_renderThread.join();
        }
    }

    void OpenGLBackend::waitForIdle()
    {
        std::unique_lock<std::mutex> lock(m_frameMutex);
        m_frameCompleteCV.wait(lock,
                               [this] { return m_lastCompletedFrame >= m_currentFrameNumber; });
    }

    void OpenGLBackend::renderThreadMain()
    {
        initializeRenderThread();

        while (m_running.load()) {
            RenderFrame frame;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_queueCV.wait(lock, [this] { return !m_frameQueue.empty() || !m_running.load(); });

                if (!m_running.load() && m_frameQueue.empty()) {
                    std::cout << "Render thread: exiting loop" << std::endl;
                    break;
                }

                if (!m_frameQueue.empty()) {
                    frame = std::move(m_frameQueue.front());
                    m_frameQueue.pop();
                    m_queueCV.notify_one();
                }
            }

            if (!frame.commands.empty()) {
                executeFrame(frame);
                {
                    std::lock_guard<std::mutex> lock(m_frameMutex);
                    m_lastCompletedFrame = frame.frameNumber;
                }
                m_frameCompleteCV.notify_all();
            }
        }

        shutdownRenderThread();
        m_threadExited.store(true);
        std::cout.flush();
    }

    void OpenGLBackend::initializeRenderThread()
    {
        m_surface->makeCurrent();

        auto loader = reinterpret_cast<GLADloadfunc>(m_winProcAddress);
        int v = gladLoadGL(loader);
        if (v == 0) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return;
        }

        checkGLError("setProcAddress");

        // Check OpenGL version
        const auto* glVersion = (const char*)glGetString(GL_VERSION);
        std::cout << "OpenGL Version (Render Thread): " << glVersion << '\n';

        m_matManager.setShaderManager(&m_shaderManager);

        m_renderPipeline.initialize(this, 0, 0);

        std::cout << "Creating common uniforms..." << '\n';

        m_perFrameUBOData = createUniform<CommonUniforms>();
        m_perFrameUBOData->setValue({Mat4::identity(), Mat4::identity(), Mat4::identity()});

        m_perDrawUBOData = createUniform<DrawUniforms>();
        m_perDrawUBOData->setValue(DrawUniforms{Mat4::identity(), Mat4::identity()});

        m_lightingUBO = createUniform<LightingData>();

        m_uniManager.buildUniforms(
            {m_perFrameUBOData.get(), m_perDrawUBOData.get(), m_lightingUBO.get()});

        std::cout << "=========================\n" << '\n';

        m_currentState.init();

        // Create default shader
        defaultShaderProgram = m_shaderManager.createDefaultShaderProgram();
        if (defaultShaderProgram == 0) {
            std::cerr << "Failed to create default shader program" << '\n';
            m_initialized.store(false);
            return;
        }

        m_initialized.store(true);
    }

    void OpenGLBackend::shutdownRenderThread()
    {

        // Delete default shader
        if (defaultShaderProgram != 0) {
            glDeleteProgram(defaultShaderProgram);
            defaultShaderProgram = 0;
        }

        m_surface->doneCurrent();
    }

    // ============================================================================
    // OPTIMIZED COMMAND EXECUTION
    // ============================================================================

    void OpenGLBackend::executeCommands(const std::vector<RenderCommand>& commands)
    {
        if (commands.empty())
            return;

        uint64_t frameNum = ++m_currentFrameNumber;
        RenderFrame frame(std::vector<RenderCommand>(commands), frameNum);

        // Submit to render thread
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);

            // Wait if queue is full (backpressure)
            m_queueCV.wait(lock, [this] {
                return m_frameQueue.size() < MAX_QUEUED_FRAMES || !m_running.load();
            });

            if (!m_running.load()) {
                return; // Shutting down
            }

            m_frameQueue.push(std::move(frame));
        }

        // Notify render thread
        m_queueCV.notify_one();
    }

    void OpenGLBackend::executeFrame(const RenderFrame& frame)
    {
        m_uploadQueue.processUploads(2.0f);

        std::vector<SortedDrawCommand> opaqueDraws;
        std::vector<SortedDrawCommand> transparentDraws;
        std::vector<SortedDrawCommand> skyboxDraws;

        for (const auto& cmd : frame.commands) {
            switch (cmd.type) {
                case RenderCommand::SET_VIEWPORT: {
                    int x, y, width, height;
                    cmd.viewport->getViewport(x, y, width, height);
                    glViewport(x, y, width, height);
                    m_perFrameUBOData->setValue(
                        {cmd.viewMatrix, cmd.projectionMatrix, cmd.viewMatrix.inverse()});
                    m_renderPipeline.resize(width, height);
                } break;

                case RenderCommand::SET_LIGHTS:
                    setLights(cmd.lights);
                    break;

                case RenderCommand::DRAW_MESH: {
                    SortedDrawCommand sorted;
                    sorted.command = cmd;
                    //sorted.sortKey = 0;

                    //if (blendEnabled) {
                        //transparentDraws.push_back(sorted);
                    //} else {
                        opaqueDraws.push_back(sorted);
                    //}
                } break;
                case RenderCommand::DRAW_SKYBOX: {
                    SortedDrawCommand sorted;
                    sorted.command = cmd;
                    skyboxDraws.push_back(sorted);
                } break;
                default:
                    break;
            }
        }

        // Execute the deferred pipeline
        m_renderPipeline.execute(opaqueDraws, transparentDraws, skyboxDraws);

        m_surface->swapBuffers();
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

        if (cmd.material.isValid()) {
            matData = m_matManager.getOrCreateMaterialData(cmd.material);
            if (matData && matData->isCompiled) {
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
        if (matData && matData->shaderProgram) {
            shaderProgram = matData->shaderProgram;
        }
        // Use upper bits of program ID for better distribution
        key.shader = (shaderProgram >> 4) & 0xFFFF;

        // Material ID (for batching)
        key.material = cmd.material.isValid() ? (cmd.material.index & 0xFFF) : 0;

        // Mesh ID (for batching)
        key.mesh = cmd.mesh.index & 0xFFF;

        return key;
    }

    // ============================================================================
    // BATCHED DRAWING (State Change Minimization)
    // ============================================================================

    void OpenGLBackend::setLights(const std::vector<Light>& lights)
    {
        m_cachedLights = lights;
        m_lightsDirty = true;
    }

    void OpenGLBackend::bindShaderProgram(GLuint program)
    {
        // Only call glUseProgram if we're switching to a different shader
        if (currentShaderProgram != program) {
            glUseProgram(program);
            currentShaderProgram = program;
        }
    }

    void OpenGLBackend::drawBatchedCommands(const std::vector<SortedDrawCommand>& sortedDraws)
    {
        for (const auto& draw : sortedDraws) {
            const auto& cmd = draw.command;

            // Get mesh data
            OpenGLMeshData* meshData = m_meshManager.getOrCreateMeshData(cmd.mesh);
            if (!meshData || !meshData->isUploaded) {
                continue;
            }

            // Get material data and compile if needed
            OpenGLMaterialData* matData = m_matManager.getOrCreateMaterialData(cmd.material);
            if (!m_matManager.compileMaterial(cmd.material, matData)) {
                continue;
            }

            bool waitingOnTextures = false;
            for (auto const& tex : matData->textures)
            {
                if (m_uploadQueue.isQueued(tex.second))
                {
                    // Texture upload is pending, skip this draw
                    waitingOnTextures = true;
                    break;
                }
            }
            if (waitingOnTextures)
                continue;

            // Get shader data
            OpenGLShaderData* shaderData = m_shaderManager.getShaderData(matData->shaderProgram);
            if (!shaderData) {
                continue;
            }

            auto* shaderAsset = AssetManager::instance().get<
                ShaderAsset>(shaderData->shaderAssetHandle);

            GLuint vao = m_meshManager.getOrCreateVAO(
                meshData,
                matData->shaderProgram,
                shaderAsset
                );

            if (vao == 0) {
                std::cerr << "ERROR: Failed to create VAO for mesh!" << std::endl;
                continue; // Skip this draw
            }

             glBindVertexArray(vao);

            // Bind shader program
            bindShaderProgram(matData->shaderProgram);

            // Update per-draw uniforms
            updatePerDrawUniforms(cmd, shaderData);
            updateLightUniforms(shaderData);

            // Apply material uniforms
            applyMaterialState(matData, shaderData);

            m_currentState.updateDepthState(draw.command.pipelineState.depthStencilState);
            m_currentState.updateRasterizerState(draw.command.pipelineState.rasterState);

            // Draw
            if (meshData->indexCount > 0) {
                glDrawElements(GL_TRIANGLES,
                               cmd.indexCount,
                               GL_UNSIGNED_INT,
                               (void*)(cmd.indexOffset * sizeof(uint32_t)));
            } else {
                glDrawArrays(GL_TRIANGLES, 0, meshData->vertexCount);
            }
        }
        glBindVertexArray(0);
    }

    // ============================================================================
    // UNIFORM BUFFER OBJECTS (Fast Uniform Updates)
    // ============================================================================

    void OpenGLBackend::updatePerFrameUniforms()
    {
        // Update
        m_uniManager.updateUniform(*m_perFrameUBOData);

        // Get the buffer
        auto* glBuffer = m_uniManager.getUniformBufferData(*m_perFrameUBOData);
        if (glBuffer) {
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, glBuffer->ubo);
        }
    }

    void OpenGLBackend::updatePerDrawUniforms(const RenderCommand& cmd,
                                              const OpenGLShaderData* shaderData)
    {
        if (!shaderData)
            return;

        DrawUniforms uniforms{};
        uniforms.model = cmd.transform;
        Mat3 normalMat = toMat3(cmd.transform).inverse().transpose();
        uniforms.normalMatrix = toMat4(normalMat);

        m_perDrawUBOData->setValue(uniforms);
        m_uniManager.updateUniform(*m_perDrawUBOData);

        auto* ub = m_uniManager.getUniformBufferData(*m_perDrawUBOData);
        if (ub) {
            glBindBufferBase(GL_UNIFORM_BUFFER, shaderData->perDrawUBOBinding, ub->ubo);
        }
    }

    void OpenGLBackend::updateLightUniforms(const OpenGLShaderData* shaderData)
    {
        if (m_lightsDirty) {
            LightingData lightingData{};
            lightingData.numLights = std::min<int>(static_cast<int>(m_cachedLights.size()),
                                                   MAX_LIGHTS);
            for (int i = 0; i < m_cachedLights.size(); ++i) {
                lightingData.lights[i] = m_cachedLights[i];
            }
            m_lightsDirty = false;

            auto lightingsize = sizeof(lightingData);
            m_lightingUBO->setValue(lightingData);
            m_uniManager.updateUniform(*m_lightingUBO);
        }

        if (auto* glBuffer = m_uniManager.getUniformBufferData(*m_lightingUBO)) {
            glBindBufferBase(GL_UNIFORM_BUFFER, shaderData->lightingUBOBinding, glBuffer->ubo);
        }
    }

    void OpenGLBackend::setUniformValue(GLint location,
                                        const std::unique_ptr<UniformBufferBase>& uniform)
    {
        if (location == -1 || !uniform)
            return;
        switch (uniform->getType()) {
            case UniformType::FLOAT: {
                float value;
                uniform->getData(&value, sizeof(float));
                glUniform1f(location, value);
            }
            break;
            case UniformType::VEC2: {
                Vec2 value;
                uniform->getData(&value, sizeof(Vec2));
                glUniform2fv(location, 1, &value[0]);
            }
            break;
            case UniformType::VEC3: {
                Vec3 value;
                uniform->getData(&value, sizeof(Vec3));
                glUniform3fv(location, 1, &value[0]);
            }
            break;
            case UniformType::VEC4: {
                Vec4 value;
                uniform->getData(&value, sizeof(Vec4));
                glUniform4fv(location, 1, &value[0]);
            }
            break;
            case UniformType::MAT4: {
                Mat4 value;
                uniform->getData(&value, sizeof(Mat4));
                glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
            }
            break;
            case UniformType::INT: {
                int value;
                uniform->getData(&value, sizeof(int));
                glUniform1i(location, value);
            }
            break;
            case UniformType::BOOL: {
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
        if (!matData || !shaderData)
            return;

        // Bind textures
        GLuint textureUnit = 0;

        for (const auto& [name, texHandle] : matData->textures) {
            if (!texHandle.isValid()) continue;

            // Get or create texture data
            OpenGLTextureData* texData = m_texManager.getOrCreateTextureData(texHandle);
            if (!texData) continue;

            // Schedule upload if not ready
            if (!texData->isUploaded) {
                if (!m_uploadQueue.isQueued(texHandle)) {
                    m_uploadQueue.requestUpload(texHandle,
                                               (void*)texData,
                                               UploadType::TEXTURE,
                                               (void*)&m_texManager);
                }
                continue;
            }

            // Activate texture unit
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(texData->target, texData->texture);

            // Set sampler uniform
            auto it = shaderData->uniformLocations.find(name);
            if (it != shaderData->uniformLocations.end()) {
                glUniform1i(it->second, textureUnit);
            }

            textureUnit++;

            // Limit to 16 texture units (common minimum)
            if (textureUnit >= 16) {
                std::cerr << "Warning: Material uses more than 16 textures!" << std::endl;
                break;
            }
        }

        for (const auto& [name, uniform] : matData->uniforms) {
            auto it = shaderData->uniformLocations.find(name);
            if (it != shaderData->uniformLocations.end()) {
                setUniformValue(it->second, uniform);
            }
        }
    }
}

CHAI_PLUGIN_SERVICES(OpenGLPlugin)
{
    CHAI_SERVICE_AS(chai::brew::Renderer, chai::brew::OpenGLBackend, "Renderer");
}

CHAI_DEFINE_PLUGIN_ENTRY(OpenGLPlugin, "OpenGLRenderer", "1.0.0", OPENGLRENDERER_EXPORT)
