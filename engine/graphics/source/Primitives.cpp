#include <Primitives.h>

namespace chai::gfx
{
    MeshAsset makeCube(float size)
    {
        const float h = size * 0.5f;

        // Each face: outward normal, tangent (U dir), bitangent (V dir).
        // u,v,n form a right-handed basis (u x v == n), so tangent.w == +1 everywhere.
        struct Face {
            Vec3 n, u, v;
        };
        const Face faces[6] = {
            {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}},  // +X
            {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},  // -X
            {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}},  // +Y
            {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}},  // -Y
            {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},   // +Z
            {{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}}, // -Z
        };

        MeshAsset m;
        m.vertices.reserve(24);
        m.indices.reserve(36);

        for (const Face& f : faces) {
            const uint32_t base = static_cast<uint32_t>(m.vertices.size());
            const Vec3 center = f.n * h;
            // 4 corners CCW viewed from outside: BL, BR, TR, TL
            const Vec2 uvs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
            const float su[4] = {-1, 1, 1, -1};
            const float sv[4] = {-1, -1, 1, 1};
            for (int i = 0; i < 4; ++i) {
                Vertex vert;
                vert.position = center + f.u * (su[i] * h) + f.v * (sv[i] * h);
                vert.normal = f.n;
                vert.uv = uvs[i];
                vert.tangent = Vec4{f.u.x, f.u.y, f.u.z, 1.0f};
                m.vertices.push_back(vert);
            }
            // two triangles, CCW front face
            m.indices.insert(m.indices.end(),
                             {base + 0, base + 1, base + 2, base + 0, base + 2, base + 3});
        }
        return m;
    }

} // namespace chai::gfx