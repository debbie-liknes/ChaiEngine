#pragma once

#include <Plugin/PluginLoader.h>
#include <Runtime/Engine.h>
#include <DiagnosticsUI/LogPanel.h>
#include <SpdLogSink.h>

#include <memory>

namespace chai
{
    class Editor
    {
    public:
        //lifecycle methods
        void startup();
        void shutdown();
        void requestStop();
        void run();

    private:
        std::unique_ptr<PluginLoader> loader_ = std::make_unique<PluginLoader>();
        std::unique_ptr<Engine> engine_ = std::make_unique<Engine>();
        
        std::unique_ptr<SpdLogSink> logSink_ = std::make_unique<SpdLogSink>();
        std::unique_ptr<diagnostics::GuiLogSink> guiSink_ =
            std::make_unique<diagnostics::GuiLogSink>();
    };
} // namespace chai
