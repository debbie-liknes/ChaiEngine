# Chai Physics Engine

A physics engine describes interactions between a set of objects and the forces that act on them.

The current design is based almost solely on the work described here: https://winter.dev/articles/physics-engine

In Chai, we model this set of objects with a container of `CollisionObject` types. When these objects interact with each other, we describe it with a `Collision` that provides us with the details to resolve the collision.

We focus on rigid body simulation in Chai for simplicity and applicability to most games.

## Dynamics

Scene objects are updated on a Semi-Implicit Euler method modeling of dynamics:

```
velocity += acceleration * dt
position += velocity * dt
```

This system has a tendency to dampen runaway velocities by accounting for acceleration first, thus being more predictable and stable–less likely to explode.

As a simplification, the engine nets a zero force at the end of each tick:

```
void tick() {
  ...
  Force = vec3(0, 0, 0)
}
```

The physics engine ticks at a (potentially) slower rate than the rendering engine; i.e., 60 Hz. Linear interpolation is used when querying object positions to avoid jitter in object positions.

## Collision Detection

Collision detection begins with dynamic Bounding Volume Hierarchies (BVHs), which organize the scene into a tree of bounding volumes. This allows us to quickly eliminate large regions of space that cannot possibly contain collisions, reducing the number of expensive narrow-phase checks. $\log_2(n)$ for $n$ objects. (See https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies)

For actual shape intersection testing, we use the Gilbert–Johnson–Keerthi (GJK) algorithm, which efficiently detects collisions between convex shapes—the most common types in games (e.g., boxes, spheres, tetrahedrons).

To further optimize performance, we implement a special-case path for oriented bounding boxes (OBBs). These use the Separating Axis Theorem (SAT), which provides a faster and simpler test specifically tailored to box-on-box collisions.

Concave shapes need to be wrapped in a convex bounding volume to maintain performance.

## Collision Response

Collision response is solved with the Expanding Polytope Algorithm (EPA) for detections collected via GJK. OBB responses are a short resolution computation from the SAT.
