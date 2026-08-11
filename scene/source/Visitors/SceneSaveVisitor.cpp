
#include <fstream>

#include <Log.h>

#include <Components/CameraComponent.h>
#include <Scene/GameObject.h>
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


        if (CameraComponent* camera{ dynamic_cast<CameraComponent*>(comp) })
            visit_camera(*camera);

    }

    void SceneSaveVisitor::visit_camera(CameraComponent& camera) {

        rapidxml::xml_node<>& node{ *impl_->node_ };

        rapidxml::xml_document<>& doc{ impl_->doc_ };

        rapidxml::xml_node<>* obj{ doc.allocate_node(rapidxml::node_element, "component") };
        // type
        {
            rapidxml::xml_attribute<>* type_attribute{ doc.allocate_attribute("type", "Camera") };
            obj->append_attribute(type_attribute);
        }

        const Camera& cam{ camera.getCamera() };

        // aspect ratio
        {
            std::string aspect_ratio = std::to_string(cam.getAspectRatio());
            char* allocated_string{ doc.allocate_string(aspect_ratio.data()) };

            rapidxml::xml_attribute<>* aspect_ratio_attribute{ doc.allocate_attribute("aspect_ratio", allocated_string) };
            obj->append_attribute(aspect_ratio_attribute);

        }

        // fovy
        {
            std::string fov_y = std::to_string(cam.getFovY());
            char* allocated_string{ doc.allocate_string(fov_y.data()) };

            rapidxml::xml_attribute<>* fov_y_attribute{ doc.allocate_attribute("fovy", allocated_string) };
            obj->append_attribute(fov_y_attribute);
        }

        // near plane
        {
            std::string near_plane = std::to_string(cam.getNearPlane());
            char* allocated_string{ doc.allocate_string(near_plane.data()) };

            rapidxml::xml_attribute<>* near_plane_attribute{ doc.allocate_attribute("near_plane", allocated_string) };
            obj->append_attribute(near_plane_attribute);
        }

        // far plane
        {
            std::string far_plane = std::to_string(cam.getFarPlane());
            char* allocated_string{ doc.allocate_string(far_plane.data()) };

            rapidxml::xml_attribute<>* far_plane_attribute{ doc.allocate_attribute("far_plane", allocated_string) };
            obj->append_attribute(far_plane_attribute);
        }

        // view matrix
        {
            // this is a bit silly... need a better solution for serializing the matrix

            const math::Mat4& mat{ cam.getViewMatrix() };

            std::string str;
            bool not_first{ false };
            for (auto m : mat) {

                if (not_first)
                    str += ",";

                not_first = true;
                str += std::to_string(m);

            }

            char* allocated_string{ doc.allocate_string(str.data()) };

            rapidxml::xml_attribute<>* view_matrix_attribute{ doc.allocate_attribute("view_matrix", allocated_string) };
            obj->append_attribute(view_matrix_attribute);
        }

        node.append_node(obj);

    }


    void SceneSaveVisitor::write(const std::filesystem::path& path) const {

        if (!impl_->root_)
            return;

        std::ofstream ofile{ path, std::ios::binary };
        if (!ofile.is_open()) {
            CHAI_LOG_ERROR("Could not save scene to file: {}", path.string());
            return;
        }

        ofile << impl_->doc_;

    }


}

