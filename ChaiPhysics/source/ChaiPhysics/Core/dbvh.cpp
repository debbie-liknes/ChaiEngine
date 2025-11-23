#include <ChaiPhysics/Core/dbvh.h>

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

    //// \brief Evaluates heuristic cost based on surface area of the node
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
                    return lhs.second > rhs.second;
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

        std::span<aabb> getOverlappingPairs() const
        {
            return {};
        }

        void insert(const BoxCollider* obj)
        {
            // Add padding to bounds
            auto worldBounds = obj->getWorldBounds();
            auto enlargedBounds = worldBounds;
            enlargedBounds.width += 5.f;
            enlargedBounds.height += 5.f;
            enlargedBounds.length += 5.f;

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

        void remove(const BoxCollider* obj)
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
        std::vector<aabb> m_overlappingPairsCache;

        //std::array<std::byte, 512 * 1024> m_buffer;
        //std::pmr::monotonic_buffer_resource m_nodePool;
        dbvh::Node* m_root = nullptr;

        std::unique_ptr<ICostHeuristic> m_heuristic;
    };

    dbvh::dbvh() : m_p(std::make_unique<impl>(std::make_unique<SurfaceAreaHeuristic>()))
    {

    }

    dbvh::~dbvh() = default;

    void dbvh::insert(const BoxCollider* obj)
    {
        m_p->insert(obj);
    }

    void dbvh::remove(const BoxCollider* obj)
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

    std::span<aabb> dbvh::getOverlappingPairs() const
    {
        return m_p->getOverlappingPairs();
    }
}
