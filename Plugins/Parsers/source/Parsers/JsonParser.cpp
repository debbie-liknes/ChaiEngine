#include <Parsers/JsonParser.h>
#include <json.hpp>
#include <fstream>
using json = nlohmann::json;


namespace chai
{
    JsonParser::JsonParser()
    {

    }

    bool JsonParser::CanLoad(const std::string& ext) const
    {
        return ext == ".json";
    }

    std::shared_ptr<IResource> JsonParser::Load(const std::string& path)
    {
        std::ifstream f("example.json");
        json data = json::parse(f);

        //if (!m_renderer) return nullptr;

        //int w, h, channels;
        //stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
        //if (!pixels) return nullptr;

        //auto backend = m_renderer->createTexture2D(pixels, w, h);
        //auto tex = std::make_shared<Texture2D>(Texture2D{ backend, (uint32_t)w, (uint32_t)h});
        //stbi_image_free(pixels);
        //return tex;
        return nullptr;
    }
}