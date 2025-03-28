#pragma once
#include <WindowModuleExport.h>
#include <Core/MemoryTypes.h>

namespace chai
{
    //class SharedViewer;

    class WINDOWMODULE_EXPORT Viewport
    {
    public:
        Viewport(int posX, int posY, int width, int height);
        ~Viewport() = default;

        // Viewer management
        //void SetViewer(SharedViewer viewer);
        //SharedViewer GetViewer() const;

        // Rendering
        //virtual void Render() = 0; // Renders the scene using the assigned viewer

        // Accessors
        void SetDimensions(int x, int y, int width, int height);
        void GetDimensions(int& x, int& y, int& width, int& height) const;

    private:
        int m_x, m_y, m_width, m_height;
        //SharedViewer m_viewer;
    };
    typedef std::shared_ptr<Viewport> SharedViewport;
}