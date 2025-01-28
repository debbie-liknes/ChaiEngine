#pragma once
#include <ChaiApiExport.h>
#include <Engine/Common.h>

namespace CGraphics
{
    //top level container, responsible for interacting with the OS, polling for events
    //can have 0 or more viewports
    class CHAIAPI_EXPORT Window
    {
    public:

        Window(Core::CString title, int width = 1280, int height = 720);
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
        Core::CVector<SharedViewport> GetViewports();

        // Input handling
        virtual void PollEvents() = 0;

        // Accessors
        int GetWidth() const;
        int GetHeight() const;
        Core::CString GetTitle() const;

    protected:
        Core::CString m_title;

        //a flat list of viewports
        //TODO: should this be a map?
        Core::CVector<SharedViewport> m_viewports;

        //dimensions
        int m_width;
        int m_height;
    };
}