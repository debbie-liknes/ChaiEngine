#pragma once
#include <WindowModuleExport.h>
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>

namespace chai
{
    //top level container, responsible for interacting with the OS, polling for events
    //can have 0 or more viewports
    using SharedViewport = std::shared_ptr<class Viewport>;
    class WINDOWMODULE_EXPORT Window
    {
    public:

        Window(chai::CString title, int width = 1280, int height = 720);
        ~Window() = default;

        //Display Window
        virtual void Init() {}
        virtual bool Show() = 0;
        virtual void Close() = 0;
        virtual void Resize(int width, int height) = 0;

        typedef void* (*WindowProc)(const char* name);
        virtual WindowProc getProcAddress() = 0;

        // Viewport management
        void AddViewport(SharedViewport viewport);
        void RemoveViewport(SharedViewport viewport);

        //i dont love returning a vector
        chai::CVector<SharedViewport> GetViewports();

        // Input handling
        virtual void PollEvents() = 0;

        // Accessors
        int GetWidth() const;
        int GetHeight() const;
        chai::CString GetTitle() const;

    protected:
        chai::CString m_title;

        //a flat list of viewports
        //TODO: should this be a map?
        chai::CVector<SharedViewport> m_viewports;
        SharedViewport m_mainViewport;

        //dimensions
        int m_width;
        int m_height;
    };
}