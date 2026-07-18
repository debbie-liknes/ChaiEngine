/**
 * @file InternalChaiUI.h
 * @brief UI for internals and tooling. NOT for production use.
 */
#pragma once
#include <string>
#include <vector>
#include <ChaiMath.h>

namespace chai::ui
{
    // this is imgui render backend interface. We will for now provide the implementation in the vulkan backend
    class IInternalChaiUi
    {
    public:
        virtual ~IInternalChaiUi() = default;
        virtual bool initializeUI() = 0;
        virtual void shutdownUI() = 0;
    };

    class TreeNode
    {
    public:
        explicit TreeNode(std::string_view label);
        ~TreeNode();

        operator bool() const { return m_open; }

    private:
        bool m_open;
    };

    void Text(const std::string& label);
    bool Button(const std::string& label);
}