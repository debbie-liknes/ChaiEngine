#include <UI/Editor/MenuService.h>
#include <imgui.h>
#include <sstream>

namespace chai::ui
{
    static std::vector<std::string> splitPath(const std::string& path)
    {
        std::vector<std::string> parts;
        std::stringstream ss(path);
        std::string seg;
        while (std::getline(ss, seg, '/'))
            parts.push_back(seg);
        return parts;
    }

    MenuService::Node&
    MenuService::findOrCreate(Node& parent, const std::vector<std::string>& segments, size_t idx)
    {
        auto it = std::find_if(parent.children.begin(), parent.children.end(), [&](Node& n) {
            return n.label == segments[idx];
        });
        Node& child = (it != parent.children.end())
                          ? *it
                          : parent.children.emplace_back(Node{segments[idx], std::nullopt, {}});
        if (idx + 1 == segments.size())
            return child;
        return findOrCreate(child, segments, idx + 1);
    }

    void MenuService::registerItem(const std::string& path, MenuLeaf leaf)
    {
        auto segments = splitPath(path);
        Node& node = findOrCreate(root_, segments, 0);
        node.leaf = std::move(leaf);
    }

    void MenuService::unregisterItem(const std::string& path)
    {
        //TODO: probably need this when we teardown plugins
    }

    void MenuService::drawNode(const MenuService::Node& node, PanelRegistry& registry)
    {
        for (auto& child : node.children) {
            if (child.leaf) {
                if (auto* toggle = std::get_if<TogglePanel>(&*child.leaf)) {
                    bool visible = registry.isVisible(toggle->panelId);
                    if (ImGui::MenuItem(child.label.c_str(), nullptr, &visible))
                        registry.setPanelVisible(toggle->panelId, visible);
                } else if (auto* action = std::get_if<MenuAction>(&*child.leaf)) {
                    if (ImGui::MenuItem(child.label.c_str()))
                        action->onClick();
                } else if (std::get_if<MenuSeparator>(&*child.leaf)) {
                    ImGui::Separator();
                }
            } else if (!child.children.empty()) {
                if (ImGui::BeginMenu(child.label.c_str())) {
                    drawNode(child, registry);
                    ImGui::EndMenu();
                }
            }
        }
    }

    void MenuService::draw(PanelRegistry& registry)
    {
        if (!ImGui::BeginMainMenuBar())
            return;
        drawNode(root_, registry);
        ImGui::EndMainMenuBar();
    }
} // namespace chai::ui