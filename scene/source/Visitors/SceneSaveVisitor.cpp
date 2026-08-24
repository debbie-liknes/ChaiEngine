
#include <fstream>
#include <ranges>
#include <unordered_set> // maybe should use vector and ranges::find instead?

#include <Log.h>

#include <Components/CameraComponent.h>
#include <Scene/GameObject.h>
#include <TypeInfo.h>
#include <Visitors/SceneSaveVisitor.h>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>


namespace chai::scene
{

    struct SceneSaveVisitor::p {

        p() :
            root_{ doc_.allocate_node(rapidxml::node_element, "scene") }
        {
            doc_.append_node(root_);
        }

        ~p() = default;

        rapidxml::xml_document<> doc_;

        // root "scene" node
        rapidxml::xml_node<>* root_{ nullptr };

        // last visited GameObject (used when visiting Components)
        rapidxml::xml_node<>* object_{ nullptr };

        // last visited Component (used when visiting properties)
        rapidxml::xml_node<>* component_{ nullptr };

        void visit(const std::any& a, const TypeInfo::PropertyInfo& info) {}

        void visit(Component* component_object, const TypeInfo::PropertyInfo& info) {

            rapidxml::xml_node<>* current_node{ component_ };

            char* allocated_string{ doc_.allocate_string(info.name.c_str()) };
            rapidxml::xml_node<>* node{ doc_.allocate_node(rapidxml::node_element, allocated_string) };
            current_node->append_node(node);

            std::any prop{ info.getter(component_object) };

            if (const auto pti{ TypeRegistry::instance().getType(info.type) }) {

                const TypeInfo& ti{ *pti };

                for (const TypeInfo::PropertyInfo& info : ti.properties | std::views::values)
                    visit(prop, info);
            }

            component_ = current_node;

        }

    };

    SceneSaveVisitor::SceneSaveVisitor() :
        impl_{ std::make_unique<p>() }
    {}

    SceneSaveVisitor::~SceneSaveVisitor() = default;

    void SceneSaveVisitor::visit(GameObject* obj) {

        rapidxml::xml_document<>& doc{ impl_->doc_ };
        rapidxml::xml_node<>& root{ *impl_->root_ };

        std::string obj_name{ obj->getObjectName() };
        char* allocated_string{ doc.allocate_string(obj_name.c_str()) };
        rapidxml::xml_node<>* node{ impl_->doc_.allocate_node(rapidxml::node_element, allocated_string) };
        root.append_node(node);
        impl_->object_ = node;

    }

    void SceneSaveVisitor::visit(Component* comp) {

        if (const auto pti{ TypeRegistry::instance().getType(std::type_index{ typeid(*comp) }) }) {

            const TypeInfo& ti{ *pti };

            rapidxml::xml_document<>& doc{ impl_->doc_ };
            rapidxml::xml_node<>& object{ *impl_->object_ };

            char* allocated_string{ doc.allocate_string(ti.name.c_str()) };
            rapidxml::xml_node<>* node{ impl_->doc_.allocate_node(rapidxml::node_element, allocated_string) };
            object.append_node(node);
            impl_->component_ = node;

            for (const auto& [name, info] : ti.properties)
                impl_->visit(comp, info);

        } else {
            // Maybe Component should have a virtual getName() method, similar to GameObject? For now I am cheating and using std::type_info :( just to
            // give a little more helpful info
            CHAI_LOG_WARN("{} is not registered with the meta system and cannot be saved.", typeid(*comp).name());
        }

    }

    void SceneSaveVisitor::write(const std::filesystem::path& path) const {

        if (!impl_)
            return;

        std::ofstream ofile{ path, std::ios::binary };
        if (!ofile.is_open()) {
            CHAI_LOG_ERROR("could not save to file: {}", path.string());
            return;
        }

        ofile << impl_->doc_;

    }

}

