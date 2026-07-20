#include <EditorUI/SceneHierarchy.h>
#include <UI/Editor/InternalChaiUI.h>
#include <UI/Editor/TreeNode.h>
#include <Scene/Scene.h>
#include <TypeRegistry.h>
#include <UI/Editor/FontManager.h>
#include <IconsFontAwesome7.h>
#include <Scene/GameObject.h>

namespace chai::ui
{
    void drawSceneNode(scene::GameObject* obj)
    {
        TreeNode objNode(obj->getObjectName(), std::to_string(obj->getObjectId()));
        if (!objNode)
            return;

        int id = 0;
        obj->visitComponents([&](scene::Component* component) { 
            //ScopedFont componentFont(FontWeight::Medium);
            auto type = std::type_index(typeid(*component));
            auto typeInfo = TypeRegistry::instance().getType(type);
            if (typeInfo) {
                std::string label = std::string(ICON_FA_CUBE) + "  " + "MeshComponent";
                TreeNode compNode(label, std::to_string(id), ui::TreeNodeFlags::Leaf);
                //TreeNode compNode(typeInfo->name, std::to_string(id), ui::TreeNodeFlags::Leaf);
            }
            id++;
        });
        for (auto& child : obj->getChildren()) {
            drawSceneNode(child);
        }
    }

    void drawSceneHierarchy(scene::Scene& scene)
    {
/*        std::string label = std::string(ICON_FA_CUBE) + "  " + "MeshComponent";
        Text(label);*/
        for (auto& obj : scene.getObjects()) {
            //ScopedFont objFont(FontWeight::Black);
            if (!obj->getParent())
                drawSceneNode(obj.get());
        }
    }
}
