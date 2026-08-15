#include <EditorUI/SceneHierarchy.h>
#include <UI/Core/InternalChaiUI.h>
#include <UI/Core/TreeNode.h>
#include <Scene/Scene.h>
#include <TypeRegistry.h>
#include <UI/Core/FontManager.h>
#include <IconsFontAwesome7.h>
#include <Scene/GameObject.h>

namespace chai::ui
{
    const char* invalidIcon()
    {
        return ICON_FA_QUESTION;
    }

    const char* getIcon(TypeInfo::IconId id) {
        return id != NULL ? id : invalidIcon();
    }

    void drawSceneNode(scene::GameObject* obj, int& id)
    {
        auto objType = std::type_index(typeid(*obj));
        auto objInfo = TypeRegistry::instance().getType(objType);
        if (!objInfo)
            return;

        TreeNode objNode(obj->getObjectName(),
                         std::to_string(obj->id()),
                            getIcon(objInfo->icon),
                            TreeNodeFlags::SpanFullWidth | TreeNodeFlags::DefaultOpen);
        if (!objNode)
            return;

        Indent();
        Indent();
        obj->visitComponents([&](scene::Component* component) { 
            //ScopedFont componentFont(FontWeight::Medium);
            auto type = std::type_index(typeid(*component));
            auto typeInfo = TypeRegistry::instance().getType(type);
            if (typeInfo) {
                std::string componentId =
                    std::to_string(obj->id()) + "_" + std::to_string(id);
                auto icon = typeInfo->icon != "";
                TreeNode compNode(typeInfo->name,
                                  componentId,
                                  getIcon(typeInfo->icon),
                                  TreeNodeFlags::SpanFullWidth | TreeNodeFlags::Leaf |
                                      TreeNodeFlags::DrawGuideLine);
            }
            id++;
        });
        for (auto& child : obj->getChildren()) {
            drawSceneNode(child, id);
        }
        Unindent();
        Unindent();
    }

    void drawSceneHierarchy(scene::Scene& scene)
    {
        int id = 0;
        for (auto& obj : scene.getObjects()) {
            //ScopedFont objFont(FontWeight::Black);
            if (!obj->getParent())
                drawSceneNode(obj.get(), id);
        }
    }
}
