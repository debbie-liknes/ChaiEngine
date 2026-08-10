
#include <fstream>

#include <Log.h>

#include <Scene/GameObject.h>
#include <Visitors/SceneSaveVisitor.h>


namespace chai::scene
{

    void SceneSaveVisitor::visit(GameObject* node) {

        str_ += "\n";

        if (!node)
            return;

        str_ += "[node name=\"" + std::string{ node->getObjectName() } + "\" id=\"" + std::to_string(node->getObjectId()) + "\"";

        if (GameObject* parent{ node->getParent() })
            str_ += " parent=\"" + std::to_string(parent->getObjectId()) + "\"";

        str_ += "]\n";

    }

    void SceneSaveVisitor::visit(Component* node) {
        str_ += "component = todo\n";
    }

    void SceneSaveVisitor::write(const std::filesystem::path& path) const {

        std::ofstream ofile{ path, std::ios::binary };
        if (!ofile.is_open()) {
            CHAI_LOG_ERROR("Could not save scene to file: {}", path.string());
            return;
        }

        ofile << str_;

    }


}

