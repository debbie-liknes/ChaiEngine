#include <EditorUI/SceneHierarchy.h>
#include <UI/InternalChaiUI.h>
#include <UI/TreeNode.h>
#include <Scene/Scene.h>
#include <TypeRegistry.h>
#include <UI/FontManager.h>
#include <IconsFontAwesome7.h>
#include <Scene/GameObject.h>

namespace chai::ui
{
    const char* iconForComponent(const std::string& typeName)
    {
        if (typeName == "MeshComponent")
            return ICON_FA_CUBE;
        if (typeName == "TransformComponent")
            return ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT;
        if (typeName == "CameraComponent")
            return ICON_FA_VIDEO;
        if (typeName == "LightComponent")
            return ICON_FA_LIGHTBULB;
        if (typeName == "ControllerComponent")
            return ICON_FA_GAMEPAD;
        if (typeName == "SkyboxComponent")
            return ICON_FA_CLOUD;
        return ICON_FA_QUESTION;
    }

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
                                  iconForComponent(typeInfo->name),
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
