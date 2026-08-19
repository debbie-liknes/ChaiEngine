#pragma once
#include <UI/Core/Container.h>
#include <string>
#include <functional>
#include <memory>

namespace chai::ui
{
    class Panel : public Container
    {
    public:
        using Container::Container;

        explicit Panel(const std::string& name);
        void draw(UIContext& context) override;

       const std::string& displayName() const;

    private:
        std::string displayName_;
    };
}