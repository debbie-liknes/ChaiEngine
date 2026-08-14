#pragma once
#include <vector>
#include <string>
#include <functional>

namespace chai::ui
{
    using DockId = unsigned int;

    struct DockSplit {
        std::string windowId;
        float ratio;
        enum class Side { Left, Right, Top, Bottom } side;
    };

    class DockspaceService
    {
    public:
        DockId begin();

        using LayoutBuilderFn = std::function<void(DockId rootId)>;
        void setDefaultLayout(std::vector<DockSplit> splits, std::string centerWindowId);
    private:
        DockId dockspaceId_ = 0;
        bool defaultLayoutBuilt_ = false;
        LayoutBuilderFn defaultLayoutFn_;
        std::vector<DockSplit> splits_;
        std::string centerWindowId_;
    };
}