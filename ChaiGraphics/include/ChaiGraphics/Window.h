#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiGraphics/Common.h>
#include <string>
#include <vector>

namespace CGraphics
{
	//top level container, responsible for interacting with the OS, polling for events
    //can have 0 or more viewports
	class CHAIGRAPHICS_EXPORT Window
	{
    public:

        Window(std::string title, int width = 1280, int height = 720);
        ~Window() = default;

        //Display Window
        virtual void Show() = 0;
        virtual void Close() = 0;
        virtual void Resize(int width, int height) = 0;

        // Viewport management
        void AddViewport(SharedViewport viewport);
        void RemoveViewport(SharedViewport viewport);

        // Input handling
        virtual void PollEvents() = 0;

        // Accessors
        int GetWidth() const;
        int GetHeight() const;
        std::string GetTitle() const;

    private:
        std::string m_title;

        //a flat list of viewports
        //TODO: should this be a map?
        std::vector<SharedViewport> m_viewports;

        //dimensions
        int m_width;
        int m_height;
	};
}