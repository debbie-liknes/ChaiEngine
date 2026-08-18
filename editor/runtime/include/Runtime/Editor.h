#pragma once

#include <Plugin/PluginLoader.h>
#include <Runtime/Engine.h>
#include <Runtime/EditorSelection.h>
#include <DiagnosticsUI/LogPanel.h>
#include <SpdLogSink.h>
#include <EditorUI/ActionManager.h>
#include <EditorUI/PanelHost.h>
#include <EditorUI/PanelRegistry.h>
#include <EditorUI/EditorViewportManager.h>
#include <EditorUI/DockspaceService.h>
#include <EditorUI/SceneHierarchy.h>
#include <EditorUI/PropertiesPane.h>
#include <Registry/SettingsRegistry.h>

#include <memory>

namespace chai
{
    class Editor
    {
    public:
        IPlugin::ServiceList providedServices() const;
        
        //lifecycle methods
        bool startup();
        void shutdown();
        void requestStop();
        void run();

    private:
        void registerActions() const;
        void setupDockspace(const ServiceLocator& locator, scene::Scene& scene);

        std::unique_ptr<PluginLoader> loader_ = std::make_unique<PluginLoader>(providedServices());
        std::unique_ptr<Engine> engine_ = std::make_unique<Engine>();
        
        std::unique_ptr<SpdLogSink> logSink_ = std::make_unique<SpdLogSink>();
        std::unique_ptr<diagnostics::GuiLogSink> guiSink_ =
            std::make_unique<diagnostics::GuiLogSink>();

        std::shared_ptr<ui::PanelRegistry> panelRegistry_;
        std::shared_ptr<ui::ActionManager> actionManager_;
        std::shared_ptr<ui::PanelHost> panelHost_;
        std::shared_ptr<ui::DockspaceService> dockspace_;
        std::shared_ptr<ui::EditorViewportManager> vpManager_;
        std::shared_ptr<settings::SettingsRegistry> settingsRegistry_;
        EditorSelection editorSelection_;
    };
} // namespace chai
