#pragma once
#include <cstdint>
#include <string_view>
#include <string>

namespace chai::ui
{
    enum class TreeNodeFlags : uint32_t {
        None = 0,
        Leaf = 1 << 0,          // cant be opened
        Selected = 1 << 1,      // draw as currently selected
        DefaultOpen = 1 << 2,   // open on first appearance
        SpanFullWidth = 1 << 3, // row highlight spans the whole available width
        DrawGuideLine = 1 << 4,
    };
    inline TreeNodeFlags operator|(TreeNodeFlags a, TreeNodeFlags b)
    {
        return static_cast<TreeNodeFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline bool hasFlag(TreeNodeFlags flags, TreeNodeFlags test)
    {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(test)) != 0;
    }

    class TreeNode
    {
    public:
        TreeNode(std::string_view label,
                 std::string_view id,
                 std::string_view icon = "",
                 TreeNodeFlags flags = TreeNodeFlags::None);
        ~TreeNode();

        operator bool() const { return open_; }
        bool clicked() const { return clicked_; }

    private:
        bool open_;
        bool clicked_;
    };
}