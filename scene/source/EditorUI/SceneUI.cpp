#include <EditorUI/SceneUI.h>
#include <IconsFontAwesome7.h>
#include <Components/CameraComponent.h>
#include <Components/ControllerComponent.h>
#include <Components/TransformComponent.h>
#include <Components/MeshComponent.h>
#include <Components/LightComponent.h>
#include <Components/SkyboxComponent.h>
#include <UI/Editor/IconRegistry.h>

namespace chai::scene
{
    void registerSceneIcons()
    {
        ui::registerIcon<CameraComponent>(ICON_FA_VIDEO);
        ui::registerIcon<ControllerComponent>(ICON_FA_GAMEPAD);
        ui::registerIcon<TransformComponent>(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT);
        ui::registerIcon<SkyboxComponent>(ICON_FA_CLOUD);
        ui::registerIcon<LightComponent>(ICON_FA_LIGHTBULB);
        ui::registerIcon<MeshComponent>(ICON_FA_CUBE);
    }
}
