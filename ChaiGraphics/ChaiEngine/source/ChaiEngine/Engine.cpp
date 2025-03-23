#include <ChaiEngine/Engine.h>
#include <ChaiEngine/Renderer.h>
#include <Window/Viewport.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <ChaiEngine/ViewData.h>
#include <RenderObjects/Cube.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace chai::brew
{
    using RegisterBackendFn = chai::brew::Renderer* (*)();

    Engine::~Engine()
    {
        //lib cleanup
        shutdown();

        if (nullptr != m_renderer)
        {
            delete m_renderer;
        }
    }

	void Engine::init(std::string backend)
	{
        std::string dllName = backend + ".dll";

#ifdef _WIN32
        m_backendLibHandle = LoadLibraryA(dllName.c_str());
        if (!m_backendLibHandle) throw std::runtime_error("Failed to load backend DLL");

        //auto registerFunc = (void(*)())GetProcAddress((HMODULE)m_backendLibHandle, "RegisterBackend");
        auto createFn = (RegisterBackendFn)GetProcAddress((HMODULE)m_backendLibHandle, "RegisterBackend");
#else
        m_backendLibHandle = dlopen(dllName.c_str(), RTLD_LAZY);
        if (!m_backendLibHandle) throw std::runtime_error("Failed to load backend so");

        auto registerFunc = (void(*)())dlsym(m_backendLibHandle, "RegisterBackend");
#endif
        
        if (!createFn) throw std::runtime_error("Backend registration function not found");
        m_renderer = createFn();
        //whats the point if we cant render
        if (!m_renderer) throw std::runtime_error("Could not create renderer");
	}

    void Engine::shutdown() {
        // TODO: clean up objects
#ifdef _WIN32
        if (m_backendLibHandle) {
            FreeLibrary((HMODULE)m_backendLibHandle);
            m_backendLibHandle = nullptr;
        }
#else
        if (m_backendLibHandle) {
            dlclose(m_backendLibHandle);
            m_backendLibHandle = nullptr;
        }
#endif
    }

    void Engine::createWindow(std::string windowTitle)
    {
        m_windows.push_back(std::make_shared<ChaiWindow>(windowTitle));
        auto& window = m_windows.back();
        chai::SharedViewport testViewport = std::make_shared<chai::Viewport>(0, 0, window->GetWidth(), window->GetHeight());
        window->AddViewport(testViewport);
        m_renderer->setProcAddress(window->getProcAddress());
    }

    void Engine::run()
    {
        chai::brew::ViewData data;
        data.view = glm::mat4(1.f);

        float fov = glm::radians(45.0f);
        float aspect = 16.0f / 9.0f;
        float near1 = 0.1f;
        float far1 = 100.0f;
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, target, up);
        glm::mat4 projection = glm::perspective(fov, aspect, near1, far1);
        data.projMat = glm::mat4(1.f);


        //test ro
        auto cube = std::make_shared<chai::brew::CubeRO>();
        chai::CVector<chai::CSharedPtr<chai::brew::RenderObject>> ros = { cube };

        auto& window = m_windows.back();
        while (window->Show())
        {
            window->swapBuffers();
            window->PollEvents();
            m_renderer->renderFrame(window.get(), ros, data);
        }

        window->Close();
    }
}