#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiGraphics/Common.h>
#include <Core/Containers.h>

namespace CGraphics
{
	//top level container, responsible for interacting with the OS, polling for events
    //can have 0 or more viewports
	class CHAIGRAPHICS_EXPORT Window
	{
    public:

        Window(Core::CString title, int width = 1280, int height = 720);
        ~Window() = default;

        //Display Window
        virtual void Init(){}
        virtual bool Show() = 0;
        virtual void Close() = 0;
        virtual void Resize(int width, int height) = 0;

        typedef void* (*WindowProc)(const char* name);
        virtual WindowProc getProcAddress() = 0;

        // Viewport management
        void AddViewport(SharedViewport viewport);
        void RemoveViewport(SharedViewport viewport);

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