# Dynamic Bounding Volume Hierarchy

Ref: https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf

Dynamic BVHs divide a scene in engines like Bullet and NVIDIA's PhysX for fast scene segmentation and raycasting in physics engines. "Dynamic" refers to the engine's ability to remove and reinsert objects into the BVH at runtime with relative efficiency.

Key differences from a normal BVH:
- "Fat" AABBs are used for single objects to provide tolerances for anticipated object movement. Otherwise, objects would need to be reinserted every frame.
- Periodically rebalancing the tree through various heuristics rather than on-insert.

## Bounding Volume Hierarchy Basics

1. Compute a bounding volume that encloses them all.
2. Use a Surface Area Heuristic to split the volume in a way that minimizes expected cost.
3. Recurse.
4. Stop when all leaves are reached.

The BVH stores the bounds of a node and its left / right children (also primitive for leaves).


We define surface area based on the bounding box that contains the primitive, which is defined as:

$S=2(wh+hd+dw)$

This functions as a cost function that we can minimize.

$C(T)=\sum_{i\in Nodes} SA(i)$

However, because the surface area of the root node and its leaves (the primitives) are not going to change when comparing trees, we can simplify this algorithm to only focus on internal nodes:

$C(T)=\sum_{i\in Inner Nodes} SA(i)$

When dealing with node insertion, we are focused on the delta in surface area produced by the new primitive insertion, which is calculated as the SA of the new parent node plus the increased surface are of all the ancestors:

$\Delta SA(node)=SA(node\cup primitive)-SA(node)$

Because calculating the global minimum cost is expensive when done exhaustively for every possible sibling, a branch and bound approach is used. A priority queue is used to 
