//#pragma once
//#include <GLFWWindowPluginExport.h>
//#include <Window/Window.h>
//
//namespace chai
//{
//	class GLFWWINDOWPLUGIN_EXPORT WindowGLFW : public Window
//	{
//    public:
//        ChaiWindow(chai::CString title, int width = 1280, int height = 720);
//        virtual ~ChaiWindow();
//
//        //Display Window
//        void Init() override;
//        bool Show() override;
//        void Close() override;
//        void Resize(int width, int height) override;
//        WindowProc getProcAddress() override;
//
//        // Input handling
//        void PollEvents() override;
//
//        //this is OpenGL specific, vulkan does it differently
//        void swapBuffers() override;
//        InputState m_state;
//    private:
//
//        GLFWwindow* m_window;
//        GLFWInfo m_userPointer;
//	};
//}