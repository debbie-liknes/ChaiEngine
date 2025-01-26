#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiGraphics/Window.h>
#include <Core/Containers.h>
#include <GLFW/glfw3.h>

namespace CGraphics
{
	//glfw window implementation
	class CHAIGRAPHICS_EXPORT ChaiWindow : public Window
	{
    public:
        ChaiWindow(Core::CString title, int width = 1280, int height = 720);
        ~ChaiWindow();

        //Display Window
        void Init() override;
        bool Show() override;
        void Close() override;
        void Resize(int width, int height) override;
        WindowProc getProcAddress() override;

        // Input handling
        void PollEvents() override;

    private:
        GLFWwindow* m_window;
	};
}