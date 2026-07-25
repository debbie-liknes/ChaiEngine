#include <EditorUI/SceneHierarchy.h>
#include <UI/Editor/InternalChaiUI.h>
#include <UI/Editor/TreeNode.h>
#include <Scene/Scene.h>
#include <TypeRegistry.h>
#include <UI/Editor/FontManager.h>
#include <IconsFontAwesome7.h>
#include <Scene/GameObject.h>
#include <UI/Editor/IconRegistry.h>

namespace chai::ui
{
    void drawSceneNode(scene::GameObject* obj, int& id)
    {
        TreeNode objNode(obj->getObjectName(),
                         std::to_string(obj->getObjectId()),
                            ICON_FA_CUBES,
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
                    std::to_string(obj->getObjectId()) + "_" + std::to_string(id);
                TreeNode compNode(typeInfo->name,
                                  componentId,
                                  iconForComponent(*typeInfo),
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
