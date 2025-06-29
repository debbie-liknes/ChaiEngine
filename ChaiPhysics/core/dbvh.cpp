#include "dbvh.h"

#include <array>
#include <vector>
#include <queue>
#include <memory_resource>

namespace chai
{
    // \brief Heuristic interface for determining cost of a node
    class ICostHeuristicVisitor
    {
    public:
        virtual ~ICostHeuristicVisitor() = default;

        virtual dbvh::Node* visit(dbvh::Node* node, dbvh::Node* newNode) const = 0;
    };

    // \brief Evaluates heuristic cost based on surface area of the node
    class SurfaceAreaHeuristicVisitor : public ICostHeuristicVisitor
    {
        dbvh::Node* visit(dbvh::Node* node, dbvh::Node* newNode) const override
        {
            double sa_node = node->bounds.getSurfaceArea();

            // TODO: Union with existing nodes to determine cost


            return node;
        }
    };

    class dbvh::impl
    {
    public:
        explicit impl(std::unique_ptr<ICostHeuristicVisitor> heuristic)
            : //m_nodePool(std::pmr::monotonic_buffer_resource(m_buffer.data(),
              //  m_buffer.size())),
              m_heuristic(std::move(heuristic))
        {
        }

        dbvh::Node* getRootNode() const
        {
            return m_root;
        }

        std::span<dbvh::aabb> getOverlappingPairs() const
        {
            return {};
        }

        void insert(dbvh::BoxCollider* obj)
        {
            // 1. Find optimal location in tree based on SAH heuristic and priority queue
            // 2. Insert at location
            // 3. Update parent nodes
            // 4. Rebalance tree? (I think this is only needed for sorted input?)

            // Root empty? Generate a parent node and insert on left
            if (m_root == nullptr)
            {
                m_root = new dbvh::Node();
                m_root->left = new dbvh::Node{
                    .bounds = obj->getWorldBounds(),
                    .object = obj
                };
            }
            else
            {
                auto newNode = new dbvh::Node{
                    .bounds = obj->getWorldBounds(),
                    .object = obj
                };

                //dbvh::Node* bestNodeToInsertAt = m_heuristic->bestParent();

            }
        }
    private:
        // We cache overlapping pairs from a given update
        // so that clients can retrieve in constant time.
        std::vector<dbvh::aabb> m_overlappingPairsCache;

        //std::array<std::byte, 512 * 1024> m_buffer;
        //std::pmr::monotonic_buffer_resource m_nodePool;
        dbvh::Node* m_root = nullptr;

        std::unique_ptr<ICostHeuristicVisitor> m_heuristic;

        // Heuristic priority queue selection
        std::priority_queue<dbvh::Node*> m_sahPriQueue;
    };

    dbvh::aabb dbvh::BoxCollider::getWorldBounds() const
    {
        // the octet cluster of points
        auto lll = center + box.center + glm::vec3(-box.width / 2.0,    -box.height / 2.0,  -box.length / 2.0);
        auto rll = center + box.center + glm::vec3( box.width / 2.0,    -box.height / 2.0,  -box.length / 2.0);
        auto lul = center + box.center + glm::vec3(-box.width / 2.0,     box.height / 2.0,  -box.length / 2.0);
        auto rul = center + box.center + glm::vec3( box.width / 2.0,     box.height / 2.0,  -box.length / 2.0);
        auto llr = center + box.center + glm::vec3(-box.width / 2.0,    -box.height / 2.0,   box.length / 2.0);
        auto rlr = center + box.center + glm::vec3( box.width / 2.0,    -box.height / 2.0,   box.length / 2.0);
        auto lur = center + box.center + glm::vec3(-box.width / 2.0,     box.height / 2.0,   box.length / 2.0);
        auto rur = center + box.center + glm::vec3( box.width / 2.0,     box.height / 2.0,   box.length / 2.0);

        auto rotatedPoints = {
            rotateAt(lll),
            rotateAt(rll),
            rotateAt(lul),
            rotateAt(rul),
            rotateAt(llr),
            rotateAt(rlr),
            rotateAt(lur),
            rotateAt(rur)
        };

        auto min_x = std::numeric_limits<double>::max();
        auto min_y = std::numeric_limits<double>::max();
        auto min_z = std::numeric_limits<double>::max();
        auto max_x = std::numeric_limits<double>::min();
        auto max_y = std::numeric_limits<double>::min();
        auto max_z = std::numeric_limits<double>::min();
        for (auto&& pt : rotatedPoints)
            if (pt.x < min_x)
                min_x = pt.x;
            else if (pt.x > max_x)
                max_x = pt.x;
        for (auto&& pt : rotatedPoints)
            if (pt.y < min_y)
                min_y = pt.y;
            else if (pt.y > max_y)
                max_y = pt.y;
        for (auto&& pt : rotatedPoints)
            if (pt.z < min_z)
                min_z = pt.z;
            else if (pt.z > max_z)
                max_z = pt.z;

        return aabb {
            glm::vec3((max_x + min_x) / 2.0, (max_y + min_y) / 2.0, (max_z + min_z) / 2.0),
            max_x - min_x,
            max_y - min_y,
            max_z - min_z
        };
    }

    double dbvh::aabb::getSurfaceArea() const
    {
        return 2.0 * (width * width + height * height + length * length);
    }

    dbvh::dbvh() : m_p(std::make_unique<impl>(std::make_unique<SurfaceAreaHeuristicVisitor>()))
    {

    }

    dbvh::~dbvh() = default;

    void dbvh::insert(dbvh::BoxCollider* obj)
    {
        m_p->insert(obj);
    }

    void dbvh::update()
    {
    }

    dbvh::Node* dbvh::getRootNode() const
    {
        return m_p->getRootNode();
    }

    std::span<dbvh::aabb> dbvh::getOverlappingPairs() const
    {
        return m_p->getOverlappingPairs();
    }
}
