#include "dbvh.h"

#include <array>
#include <vector>
#include <queue>
#include <stack>
#include <memory_resource>

namespace chai
{
    // \brief Heuristic interface for determining cost of a node
    class ICostHeuristic
    {
    public:
        virtual ~ICostHeuristic() = default;

        virtual dbvh::Node* findBestSibling(dbvh::Node* root, dbvh::Node* nodeToAdd) = 0;
    };

    // \brief Evaluates heuristic cost based on surface area of the node
    class SurfaceAreaHeuristic : public ICostHeuristic
    {
    public:
        dbvh::Node* findBestSibling(dbvh::Node* root, dbvh::Node* nodeToAdd) override
        {
            // Pair = (node, inherited cost [surface area sum up through parent of node])
            using SurfaceAreaPair = std::pair<dbvh::Node*, double>;
            class SurfaceAreaComparator
            {
            public:
                bool operator()(const SurfaceAreaPair& lhs, const SurfaceAreaPair& rhs) const
                {
                    // priority_queue is a max-heap (greater = higher priority)
                    // compare inherited costs
                    return lhs.second > lhs.second;
                }
            };
            std::priority_queue<SurfaceAreaPair, std::vector<SurfaceAreaPair>, SurfaceAreaComparator> m_saPriQueue;

            double bestCost = std::numeric_limits<double>::max();
            dbvh::Node* bestNode = nullptr;

            // Root has no inherited cost
            m_saPriQueue.push({ root, 0 });

            // Process nodes until the priority queue is empty.
            // We regulate insertions into the queue based on lower bound of cost.
            while (!m_saPriQueue.empty())
            {
                auto pair = m_saPriQueue.top();
                auto& currNode = pair.first;
                auto& inheritedCost = pair.second;
                m_saPriQueue.pop();

                double sa_node = currNode->boundsEnlarged.getSurfaceArea();
                double sa_newNode = nodeToAdd->boundsEnlarged.getSurfaceArea();

                // Determine if a union of volumes would be a better cost here.
                auto unionBounds = currNode->boundsEnlarged.expand(nodeToAdd->boundsEnlarged);
                auto unionCost = unionBounds.getSurfaceArea();
                auto newCost = inheritedCost + unionCost;
                if (newCost < bestCost)
                {
                    bestCost = newCost;
                    bestNode = currNode;
                }

                // The sum cost functions as a lower bound for the children's costs.
                // If this is less than our best cost so far, we should pursue further.
                auto lowerBoundCost = inheritedCost + sa_node + sa_newNode;
                if (lowerBoundCost < bestCost)
                {
                    m_saPriQueue.push({ currNode->left, inheritedCost + sa_node });
                    m_saPriQueue.push({ currNode->right, inheritedCost + sa_node });
                }
            }

            return bestNode;
        }
    };

    class dbvh::impl
    {
    public:
        explicit impl(std::unique_ptr<ICostHeuristic> heuristic)
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

        void insert(const dbvh::BoxCollider* obj)
        {
            // Add padding to bounds
            auto worldBounds = obj->getWorldBounds();
            auto enlargedBounds = worldBounds;
            enlargedBounds.width += 5.0;
            enlargedBounds.height += 5.0;
            enlargedBounds.length += 5.0;

            if (m_root == nullptr)
            {
                m_root = new dbvh::Node{
                    .parent = m_root,
                    .boundsEnlarged = enlargedBounds,
                    .boundsLeaf = worldBounds,
                    .object = obj
                };
            }
            else
            {
                auto newLeaf = new dbvh::Node{
                    .boundsEnlarged = enlargedBounds,
                    .boundsLeaf = worldBounds,
                    .object = obj
                };

                dbvh::Node* bestSibling = m_heuristic->findBestSibling(m_root, newLeaf);
                dbvh::Node* newParent = new dbvh::Node{
                    .left = newLeaf,
                    .right = bestSibling,
                    .parent = bestSibling->parent,
                    .boundsEnlarged = bestSibling->boundsEnlarged.expand(newLeaf->boundsEnlarged),
                    .boundsLeaf = aabb{} // Default constructed since invalid (no object pointer)
                };
                newLeaf->parent = newParent;

                // Update left / right node of sibling's parent to new parent
                if (bestSibling->parent)
                {
                    if (bestSibling->parent->left == bestSibling)
                        bestSibling->parent->left = newParent;
                    else if (bestSibling->parent->right == bestSibling)
                        bestSibling->parent->right = newParent;
                }
                else
                {
                    m_root = newParent;
                }

                bestSibling->parent = newParent;
            }
        }

        void remove(const dbvh::BoxCollider* obj)
        {
            std::stack<dbvh::Node*> dfs;
            dfs.push(m_root);
            while (!dfs.empty())
            {
                auto node = dfs.top();
                dfs.pop();

                if (node->object == obj)
                {
                    // found
                    if (node->parent)
                    {
                        // Remove our link from our parent to disconnect us from the graph.
                        if (node->parent->left == node)
                            node->parent->left = nullptr;
                        else if (node->parent->right == node)
                            node->parent->right = nullptr;

                        // NOTE: We must be a leaf node, so there are no children to remove.

                        // If parent is now empty (having no other children leaves), delete it.
                        if (node->parent->left == nullptr &&
                            node->parent->right == nullptr)
                        {
                            // Check that this parent isn't the root
                            if (node->parent->parent != nullptr)
                            {
                                // Update our parent node's links
                                if (node->parent->parent->left == node->parent)
                                    node->parent->parent->left = nullptr;
                                else if (node->parent->parent->right == node->parent)
                                    node->parent->parent->right = nullptr;
                            }
                            else
                            {
                                // Reset the root
                                m_root = nullptr;
                            }
                            delete node->parent;
                        }
                    }
                    else
                    {
                        m_root = nullptr;
                    }

                    delete node;
                    break;
                }

                if (node->right)
                    dfs.push(node->right);

                if (node->left)
                    dfs.push(node->left);
            }
        }

    private:
        // We cache overlapping pairs from a given update
        // so that clients can retrieve in constant time.
        std::vector<dbvh::aabb> m_overlappingPairsCache;

        //std::array<std::byte, 512 * 1024> m_buffer;
        //std::pmr::monotonic_buffer_resource m_nodePool;
        dbvh::Node* m_root = nullptr;

        std::unique_ptr<ICostHeuristic> m_heuristic;
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

    bool dbvh::aabb::operator==(const dbvh::aabb& rhs) const
    {
        return (
            this->center == rhs.center &&
            this->width == rhs.width &&
            this->length == rhs.length &&
            this->height == rhs.height
        );
    }

    double dbvh::aabb::getSurfaceArea() const
    {
        return 2.0 * (width * width + height * height + length * length);
    }

    dbvh::aabb dbvh::aabb::expand(const dbvh::aabb& box) const
    {
        double min_x = std::min(this->center.x - this->width / 2.0,
            box.center.x - box.width / 2.0);
        double min_y = std::min(this->center.y - this->width / 2.0,
            box.center.y - box.width / 2.0);
        double min_z = std::min(this->center.z - this->width / 2.0,
            box.center.z - box.width / 2.0);
        double max_x = std::max(this->center.x + this->width / 2.0,
            box.center.x + box.width / 2.0);
        double max_y = std::max(this->center.y + this->width / 2.0,
            box.center.y + box.width / 2.0);
        double max_z = std::max(this->center.z + this->width / 2.0,
            box.center.z + box.width / 2.0);

        return aabb {
            glm::vec3((max_x + min_x) / 2.0, (max_y + min_y) / 2.0, (max_z + min_z) / 2.0),
            max_x - min_x,
            max_y - min_y,
            max_z - min_z
        };
    }

    dbvh::dbvh() : m_p(std::make_unique<impl>(std::make_unique<SurfaceAreaHeuristic>()))
    {

    }

    dbvh::~dbvh() = default;

    void dbvh::insert(const dbvh::BoxCollider* obj)
    {
        m_p->insert(obj);
    }

    void dbvh::remove(const dbvh::BoxCollider* obj)
    {
        m_p->remove(obj);
    }

    void dbvh::update()
    {
        // TODO: Loop over every leaf node in the graph and update
        // Does the bounding volume hierarchy have knowledge of Euler dynamics?
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
