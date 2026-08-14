#include <DiagnosticsUI/LogPanel.h>
#include <imgui.h>
#include <UI/Core/InternalChaiUI.h>
#include <UI/Core/FontManager.h>

namespace chai::diagnostics
{
    void GuiLogSink::write(const LogRecord& record)
    {
        std::lock_guard lock(mutex_);
        entries_.emplace_back(record.level, std::string(record.message));
        if (entries_.size() > kMaxEntries)
            entries_.pop_front();
    }

    void GuiLogSink::getEntries(std::vector<Entry>& out) const
    {
        std::lock_guard lock(mutex_);
        out.assign(entries_.begin(), entries_.end());
    }

    void GuiLogSink::clear()
    {
        std::lock_guard lock(mutex_);
        entries_.clear();
    }

    math::Vec4 logLevelColor(chai::LogLevel level)
    {
        if (level == chai::LogLevel::Error || level == chai::LogLevel::Critical)
            return math::Vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        else if (level == chai::LogLevel::Warn)
            return math::Vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        else if (level == chai::LogLevel::Info)
            return math::Vec4(1.0f, 1.0f, 1.0f, 1.0f); // White
        else if (level == chai::LogLevel::Debug || level == chai::LogLevel::Trace)
            return math::Vec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray

        return math::Vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default to white
    }

    void drawLogPanel(GuiLogSink& sink)
    {
        static bool autoScroll = true;
        static ImGuiTextFilter filter;

        if (chai::ui::Button("Clear"))
            sink.clear();
        chai::ui::SameLine();
        chai::ui::Checkbox("Auto-scroll", autoScroll);
        chai::ui::SameLine();
        filter.Draw("Filter", -100.0f);

        chai::ui::BeginChildRegion("LogScroll");

        std::vector<GuiLogSink::Entry> entries;
        sink.getEntries(entries);

        {
            ui::ScopedFont mono(ui::FontWeight::Mono);
            for (auto& e : entries) {
                if (!filter.PassFilter(e.message.c_str()))
                    continue;

                math::Vec4 color = logLevelColor(e.level);
                chai::ui::TextColored(color, e.message);
            }
        }

        if (autoScroll && chai::ui::GetScrollY() >= chai::ui::GetScrollMaxY())
            chai::ui::SetScrollHereY(1.0f);

        chai::ui::EndChildRegion();
    }
}
