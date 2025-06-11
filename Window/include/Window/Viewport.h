#pragma once
#include <WindowModuleExport.h>
#include <string>
#include <memory>

namespace chai
{
    // Viewport descriptor
    struct WINDOWMODULE_EXPORT ViewportDesc
    {
        std::string name;
        int x, y, width, height;
        float clearColor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
        bool clearDepth = true;
        bool clearStencil = false;
    };

    // Viewport class
    class WINDOWMODULE_EXPORT Viewport
    {
    public:
        Viewport(uint64_t viewId, const ViewportDesc& desc, uint64_t window);

        void setActive();
        void clear();
        void setRect(int x, int y, int width, int height);
        void setClearColor(float r, float g, float b, float a = 1.0f);

        const ViewportDesc& getDesc() const { return m_desc; }
        const std::string& getName() const { return m_desc.name; }
        uint64_t getParentWindow() const { return m_parentWindow; }

    private:
        ViewportDesc m_desc;
        uint64_t m_parentWindow;
    };
}