#pragma once
#include <ParsersExport.h>
#include <Resource/ResourceLoader.h>
#include <Meta/ChaiMacros.h>
#include <Plugin/PluginRegistry.h>
#include <Plugin/ServiceLocator.h>


namespace chai
{
    class JsonParser : public IResourceLoader {
    public:
        JsonParser();

        bool CanLoad(const std::string& ext) const override;
        std::shared_ptr<IResource> Load(const std::string& path) override;

    private:
    };

    //CHAI_CLASS(chai::JsonParser)
    //END_CHAI()
}

CHAI_PLUGIN_CLASS(CommonParsers) {
    CHAI_SERVICE(chai::JsonParser, "JSON parser");
}
CHAI_REGISTER_PLUGIN(CommonParsers, "Parsers", "1.0.0")