#pragma once
#include <WindowModuleExport.h>
#include <Window/Window.h>
#include <Core/Containers.h>
#include <GLFW/glfw3.h>

namespace chai
{
	//glfw window implementation
	class WINDOWMODULE_EXPORT ChaiWindow : public Window
	{
    public:
        ChaiWindow(chai::CString title, int width = 1280, int height = 720);
        virtual ~ChaiWindow();

        //Display Window
        void Init() override;
        bool Show() override;
        void Close() override;
        void Resize(int width, int height) override;
        WindowProc getProcAddress() override;

        // Input handling
        void PollEvents() override;

        //this is OpenGL specific, vulkan does it differently
        void swapBuffers();
    private:

        GLFWwindow* m_window;
	};
}