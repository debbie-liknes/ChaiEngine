#include "WindowGLFW.h"

#include <GLFW/glfw3.h>

#include <Log.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <Window/Window.h>
#include <memory>

namespace chai
{
    void onGlfwError(int code, const char* desc)
    {
        CHAI_LOG_ERROR("GLFW error {}: {}", code, desc);
    }

    class WindowPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "Window(GLFW)"; }

        void onLoad(PluginContext& ctx) override
        {
            glfwSetErrorCallback(&onGlfwError);
            if (!glfwInit()) {
                CHAI_LOG_CRITICAL("glfwInit failed; no window service available");
                return;
            }

            WindowDesc desc;
            window_ = std::make_shared<WindowGLFW>(desc);
            ctx.services.provide<IWindow>(window_);
            CHAI_LOG_INFO("Window service provided");
        }

        void onUnload(PluginContext& ctx) override
        {
            ctx.services.remove<IWindow>();
            window_.reset();
            glfwTerminate();
            CHAI_LOG_INFO("Window removed, GLFW terminated");
        }

    private:
        std::shared_ptr<WindowGLFW> window_;
    };

    CHAI_PLUGIN(WindowPlugin);
}