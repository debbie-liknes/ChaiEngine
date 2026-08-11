
#pragma once

#include <filesystem>
#include <memory>

#include <Visitors/Visitor.h>

namespace chai::scene
{
    class SceneSaveVisitor : public Visitor
    {
    public:
        SceneSaveVisitor();
        ~SceneSaveVisitor();

        virtual void visit(GameObject* node) override;

        virtual void visit(Component* none) override;

        virtual void reset() override {}

        void write(const std::filesystem::path& path) const;

    private:
        struct p;
        std::unique_ptr<p> impl_;
    };
}

