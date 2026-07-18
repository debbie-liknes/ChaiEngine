#include <UI/Tools/FontManager.h>
#include <imgui.h>
#include <Log.h>

namespace chai::ui
{
	struct AppFont {
		AppFont(const std::string& path, float size = 18.f)
		{
			font_ = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size);
			if (!font_)
				CHAI_LOG_ERROR("Failed to load font '{}'", path);
		}
		void push() const
		{
			if (font_)
				ImGui::PushFont(font_);
		}
		ImFont* font_ = nullptr;
    };

	void FontManager::loadFonts(const std::string& fontDirectory)
	{
		fonts_.clear();
		fonts_[FontWeight::Regular] = new AppFont(fontDirectory + "/Inter/static/Inter_18pt-Regular.ttf");
		fonts_[FontWeight::Title] = new AppFont(fontDirectory + "/Inter/static/Inter_24pt-Regular.ttf", 24.f);
		fonts_[FontWeight::BoldTitle] = new AppFont(fontDirectory + "/Inter/static/Inter_24pt-Bold.ttf", 24.f);
		fonts_[FontWeight::Medium] = new AppFont(fontDirectory + "/Inter/static/Inter_18pt-Medium.ttf");
		fonts_[FontWeight::Bold] = new AppFont(fontDirectory + "/Inter/static/Inter_18pt-Bold.ttf");
		fonts_[FontWeight::Mono] = new AppFont(fontDirectory + "/JetBrains_Mono/static/JetBrainsMono-Regular.ttf");
	}

	void FontManager::push(FontWeight weight) const
	{
		auto it = fonts_.find(weight);
		if (it != fonts_.end())
			it->second->push();
	}

	void FontManager::pop() const
	{
		ImGui::PopFont();
	}
} // namespace chai::ui