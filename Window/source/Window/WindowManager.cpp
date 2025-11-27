#include <Window/WindowManager.h>
#include <Window/WindowSystem.h>

#include <algorithm>

namespace chai
{
    WindowManager::WindowManager(std::shared_ptr<WindowSystem> system) : m_windowSystem(system)
    {
        m_windowSystem->initialize();
    }

    WindowManager::~WindowManager()
    {
        m_windowSystem->shutdown();
    }

    WindowId WindowManager::createWindow(const WindowDesc& desc)
    {
        m_windows.push_back(std::move(m_windowSystem->createWindow(desc, this)));
        return m_windows.back()->getId();
    }

    void WindowManager::requestClose(WindowId id)
    {
        //potentially problematic for apis like vulkan
        auto win = findWindowById(id);
        if (win)
        {
            m_windowSystem->destroyWindow(win->systemWindow);
        }
        auto it = std::ranges::remove_if(m_windows,
                                         [id](const std::unique_ptr<Window>& window) { return window->getId() == id; });
        m_windows.erase(it.begin(), m_windows.end());
    }

    bool WindowManager::isDone() const
    {
        return m_windows.empty();
    }

    void WindowManager::update()
    {
        m_windowSystem->pollEvents();
        //for (auto& window : m_windows)
        //{
        //    m_windowSystem->swapBuffers(window->systemWindow);
        //}
    }

    Window* WindowManager::findWindowById(WindowId id)
    {
        auto windowItr = std::ranges::find(m_windows, id, [](const auto& window) { return window->getId(); });
        if (windowItr != m_windows.end())
            return windowItr->get();
        return nullptr;
    }

    void* WindowManager::getSystemWindow(WindowId id)
    {
        auto win = findWindowById(id);
        if (win) {
            return win->systemWindow;
        }
        return nullptr;
    }
}