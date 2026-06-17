#include "GLTFLoader.h"
#include <Log.h>
#include <FileIO.h>

// Im tired of looking at these warnings that I cant fix
// Cgltf.cpp
#pragma warning(push)
#pragma warning(disable : 4996 26449) // whatever cgltf emits

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#pragma warning(pop)


namespace chai::gfx
{
    bool GLTFLoader::canLoad(std::string_view ext) const
    {
        return ext == "gltf";
    }

    const char* GLTFLoader::name() const
    {
        return "GLTFLoader";
    }

    const cgltf_accessor*
    findAttr(const cgltf_primitive& prim, cgltf_attribute_type type, int set = 0)
    {
        for (cgltf_size i = 0; i < prim.attributes_count; i++) {
            const cgltf_attribute& a = prim.attributes[i];
            if (a.type == type && a.index == set)
                return a.data;
        }
        return nullptr;
    }


    bool readPrimitive(const cgltf_primitive& inPrim, MeshAsset& out)
    {
        const cgltf_accessor* pos = findAttr(inPrim, cgltf_attribute_type_position);
        if (!pos) {
            CHAI_LOG_WARN("cgltf: primitive has no POSITION; skipping");
            return false;
        }
        const cgltf_accessor* nrm = findAttr(inPrim, cgltf_attribute_type_normal);
        const cgltf_accessor* uv = findAttr(inPrim, cgltf_attribute_type_texcoord, 0);
        const cgltf_accessor* tan = findAttr(inPrim, cgltf_attribute_type_tangent);

        out.vertices.resize(pos->count);
        for (cgltf_size i = 0; i < pos->count; i++) {
            auto& v = out.vertices[i];
            v.normal = {0, 0, 1};
            v.uv = {0, 0};
            v.tangent = {1, 0, 0, 1};
            cgltf_accessor_read_float(pos, i, &v.position.x, 3);
            if (nrm)
                cgltf_accessor_read_float(nrm, i, &v.normal.x, 3);
            if (uv)
                cgltf_accessor_read_float(uv, i, &v.uv.x, 2);
            if (tan)
                cgltf_accessor_read_float(tan, i, &v.tangent.x, 4);
        }

        if (inPrim.indices) {
            out.indices.resize(inPrim.indices->count);
            for (cgltf_size i = 0; i < inPrim.indices->count; i++)
                out.indices[i] =
                    static_cast<std::uint32_t>(cgltf_accessor_read_index(inPrim.indices, i));
        } else {
            out.indices.resize(pos->count);
            for (cgltf_size i = 0; i < pos->count; i++)
                out.indices[i] = static_cast<std::uint32_t>(i);
        }
        return true;
    }

    void extractMeshes(const cgltf_data& data, ModelAsset& model)
    {
        model.meshes.reserve(data.meshes_count);
        for (cgltf_size i = 0; i < data.meshes_count; i++)
        {
            const cgltf_mesh& meshData = data.meshes[i];
            ModelAsset::MeshEntry entry;
            if (meshData.name)
                entry.name = meshData.name;

            for (cgltf_size pi = 0; pi < meshData.primitives_count; ++pi) {
                const cgltf_primitive& prim = meshData.primitives[pi];

                ModelAsset::Primitive primEntry;
                if (!readPrimitive(prim, primEntry.mesh))
                    continue;

                primEntry.material =
                    prim.material ? static_cast<int>(prim.material - data.materials) : -1;
                entry.primitives.push_back(primEntry);

            }
            model.meshes.push_back(entry);
        }
    }

    void
    extractImages(const cgltf_data& data, const std::filesystem::path& baseDir, ModelAsset& model)
    {
        model.images.reserve(data.images_count);
        for (cgltf_size i = 0; i < data.images_count; i++) {
            const cgltf_image& image = data.images[i];
            ModelAsset::ImageData imageOut;

            if (image.name)
                imageOut.name = image.name;
            if (image.mime_type)
                imageOut.mimeType = image.mime_type;

            if (image.buffer_view) {
                const cgltf_buffer_view* bv = image.buffer_view;
                const auto* base = static_cast<const std::uint8_t*>(bv->buffer->data);
                if (base)
                    imageOut.bytes.assign(base + bv->offset, base + bv->offset + bv->size);
            } else if (image.uri) {
                std::string uri = image.uri;
                cgltf_decode_uri(uri.data());
                uri.resize(std::strlen(uri.c_str()));
                auto fileBytes = readFileBytes(baseDir / uri);
                if (!fileBytes.empty())
                    imageOut.bytes = std::move(fileBytes);
                else
                    CHAI_LOG_ERROR("cgltf: could not read image '{}'", uri);
            }
            model.images.push_back(imageOut);
        }
    }

    int imageIndexOf(const cgltf_data& data, const cgltf_texture_view& view)
    {
        if (!view.texture || !view.texture->image)
            return -1;
        return static_cast<int>(view.texture->image - data.images);
    }


    void extractMaterials(const cgltf_data& data, ModelAsset& model)
    {
        model.materials.reserve(data.materials_count);
        for (cgltf_size i = 0; i < data.materials_count; i++) {
            static int hack = 0;
            const cgltf_material& m = data.materials[i];
            ModelAsset::MaterialDesc d;

            if (m.has_pbr_metallic_roughness) {
                const auto& pbr = m.pbr_metallic_roughness;
                d.baseColorFactor = {pbr.base_color_factor[0],
                                     pbr.base_color_factor[1],
                                     pbr.base_color_factor[2],
                                     pbr.base_color_factor[3]};
                d.metallic = pbr.metallic_factor;
                d.roughness = pbr.roughness_factor;
                d.baseColor = imageIndexOf(data, pbr.base_color_texture);
                d.metallicRoughness = imageIndexOf(data, pbr.metallic_roughness_texture);
            }
            if (m.name)
                d.name = m.name;
            d.normal = imageIndexOf(data, m.normal_texture);
            d.occlusion = imageIndexOf(data, m.occlusion_texture);
            d.emissive = imageIndexOf(data, m.emissive_texture);
            d.emissiveFactor = {m.emissive_factor[0], m.emissive_factor[1], m.emissive_factor[2]};
            d.alphaCutoff = m.alpha_cutoff;
            d.doubleSided = (m.double_sided != 0);
            switch (m.alpha_mode) {
                case cgltf_alpha_mode_mask:
                    d.alphaMode = AlphaMode::Mask;
                    break;
                case cgltf_alpha_mode_blend:
                    d.alphaMode = AlphaMode::Blend;
                    break;
                default:
                    d.alphaMode = AlphaMode::Opaque;
                    break;
            }
            model.materials.push_back(d);
        }
    }

    void decomposeMatrix(const cgltf_float m[16], math::Vec3& t, math::Quat& r, math::Vec3& s)
    {
        t = {m[12], m[13], m[14]};

        math::Vec3 c0{m[0], m[1], m[2]};
        math::Vec3 c1{m[4], m[5], m[6]};
        math::Vec3 c2{m[8], m[9], m[10]};
        s = {length(c0), length(c1), length(c2)};

        const float ix = s.x ? 1.f / s.x : 0.f;
        const float iy = s.y ? 1.f / s.y : 0.f;
        const float iz = s.z ? 1.f / s.z : 0.f;
        const float r00 = m[0] * ix, r01 = m[4] * iy, r02 = m[8] * iz;
        const float r10 = m[1] * ix, r11 = m[5] * iy, r12 = m[9] * iz;
        const float r20 = m[2] * ix, r21 = m[6] * iy, r22 = m[10] * iz;

        const float trace = r00 + r11 + r22;
        if (trace > 0.f) {
            float k = std::sqrt(trace + 1.f) * 2.f;
            r.w = 0.25f * k;
            r.x = (r21 - r12) / k;
            r.y = (r02 - r20) / k;
            r.z = (r10 - r01) / k;
        } else if (r00 > r11 && r00 > r22) {
            float k = std::sqrt(1.f + r00 - r11 - r22) * 2.f;
            r.w = (r21 - r12) / k;
            r.x = 0.25f * k;
            r.y = (r01 + r10) / k;
            r.z = (r02 + r20) / k;
        } else if (r11 > r22) {
            float k = std::sqrt(1.f + r11 - r00 - r22) * 2.f;
            r.w = (r02 - r20) / k;
            r.x = (r01 + r10) / k;
            r.y = 0.25f * k;
            r.z = (r12 + r21) / k;
        } else {
            float k = std::sqrt(1.f + r22 - r00 - r11) * 2.f;
            r.w = (r10 - r01) / k;
            r.x = (r02 + r20) / k;
            r.y = (r12 + r21) / k;
            r.z = 0.25f * k;
        }
    }


    void extractNodes(const cgltf_data& data, ModelAsset& model)
    {
        model.nodes.reserve(data.nodes_count);
        for (cgltf_size i = 0; i < data.nodes_count; i++) {
            const cgltf_node& n = data.nodes[i];
            ModelAsset::Node out;
            if (n.name)
                out.name = n.name;

            if (n.has_matrix) {
                decomposeMatrix(n.matrix, out.position, out.rotation, out.scale);
            } else {
                out.position = {n.translation[0], n.translation[1], n.translation[2]};
                out.rotation = math::Quat{
                    n.rotation[0], n.rotation[1], n.rotation[2], n.rotation[3]};
                out.scale = {n.scale[0], n.scale[1], n.scale[2]};
            }


            out.meshIndex = n.mesh ? static_cast<int>(n.mesh - data.meshes) : -1;
            out.children.reserve(n.children_count);
            for (cgltf_size c = 0; c < n.children_count; ++c)
                out.children.push_back(static_cast<int>(n.children[c] - data.nodes));

            model.nodes.push_back(std::move(out));
        }

        const cgltf_scene* scene =
            data.scene ? data.scene : (data.scenes_count ? &data.scenes[0] : nullptr);
        if (scene) {
            model.roots.reserve(scene->nodes_count);
            for (cgltf_size i = 0; i < scene->nodes_count; ++i)
                model.roots.push_back(static_cast<int>(scene->nodes[i] - data.nodes));
        } else {
            for (cgltf_size i = 0; i < data.nodes_count; ++i)
                if (!data.nodes[i].parent)
                    model.roots.push_back(static_cast<int>(i));
        }

    }

    std::optional<ModelAsset> GLTFLoader::decode(std::span<const uint8_t> bytes,
        const std::filesystem::path& baseDir)
    {
        cgltf_options options = {};
        cgltf_data* data = nullptr;

        if (cgltf_parse(&options, bytes.data(), bytes.size(), &data) != cgltf_result_success) {
            CHAI_LOG_ERROR("cgltf: parse failed");
            return std::nullopt;
        }

        const std::string base = (baseDir / "").string();
        if (cgltf_load_buffers(&options, data, base.c_str()) != cgltf_result_success) {
            CHAI_LOG_ERROR("cgltf: load_buffers failed for base '{}'", base);
            return std::nullopt;
        }
        if (cgltf_validate(data) != cgltf_result_success)
            CHAI_LOG_WARN("cgltf: validation reported issues (continuing)");

        //cgltf success, make the ModelAsset
        //Need images, materials, meshes and nodes
        ModelAsset model;
        extractMeshes(*data, model);
        extractImages(*data, baseDir, model);
        extractMaterials(*data, model);
        extractNodes(*data, model);

        CHAI_LOG_INFO("cgltf: decoded {} meshes, {} materials, {} images, {} nodes",
          model.meshes.size(),
          model.materials.size(),
          model.images.size(),
          model.nodes.size());

        return model;
    }
} // namespace chai