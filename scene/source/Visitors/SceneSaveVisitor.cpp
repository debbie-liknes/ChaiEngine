
#include <fstream>

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

        // current "node" node (used when visiting Components)
        rapidxml::xml_node<>* node_{ nullptr };

        void create_prop_element(rapidxml::xml_node<>& node, const TypeInfo& ti) {

            char* allocated_string{ doc_.allocate_string(ti.name.data()) };
            rapidxml::xml_node<>* prop{ doc_.allocate_node(rapidxml::node_element, allocated_string) };

            for (const auto& [name, property] : ti.properties) {

                // ZHR :: TODO


            }

            node.append_node(prop);

        }

    };

    SceneSaveVisitor::SceneSaveVisitor() :
        impl_{ std::make_unique<p>() }
    {}

    SceneSaveVisitor::~SceneSaveVisitor() = default;

    void SceneSaveVisitor::visit(GameObject* node) {

        if (!node || !impl_ || !impl_->root_)
            return;

        rapidxml::xml_document<>& doc{ impl_->doc_ };
        rapidxml::xml_node<>& root{ *impl_->root_ };

        rapidxml::xml_node<>* obj{ doc.allocate_node(rapidxml::node_element, "node") };

        impl_->node_ = obj;

        // name
        {
            std::string object_name{ node->getObjectName() };
            char* allocated_string{ doc.allocate_string(object_name.data()) };

            rapidxml::xml_attribute<>* name_attribute{ doc.allocate_attribute("name", allocated_string) };
            obj->append_attribute(name_attribute);
        }

        // id
        {
            std::string object_id{ std::to_string(node->getObjectId()) };
            char* allocated_string{ doc.allocate_string(object_id.data()) };

            rapidxml::xml_attribute<>* id_attribute{ doc.allocate_attribute("id", allocated_string) };
            obj->append_attribute(id_attribute);
        }

        // parent
        if (GameObject* parent{ node->getParent() }) {

            std::string parent_id{ std::to_string(parent->getObjectId()) };
            char* allocated_string{ doc.allocate_string(parent_id.data()) };

            rapidxml::xml_attribute<>* parent_id_attribute{ doc.allocate_attribute("parent_id", allocated_string) };
            obj->append_attribute(parent_id_attribute);

        }

        root.append_node(obj);

    }


    void SceneSaveVisitor::visit(Component* comp) {

        if (!comp || !impl_->node_)
            return;

        rapidxml::xml_document<>& doc{ impl_->doc_ };
        rapidxml::xml_node<>& root{ *impl_->root_ };
        rapidxml::xml_node<>& node{ *impl_->node_ };

        if (auto ti{ TypeRegistry::instance().getType(std::type_index{typeid(*comp) })}) {
            impl_->create_prop_element(node, *ti);
        } else {
            CHAI_LOG_ERROR("Component is not registered with the meta system and cannot be saved.");
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

