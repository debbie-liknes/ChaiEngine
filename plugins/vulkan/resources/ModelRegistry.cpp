#include "ModelRegistry.h"
#include <Log.h>
#include <Core/FileIO.h>
#include <Loaders/IModelLoader.h>
#include <Assets/MaterialAsset.h>
#include <Loaders/ITextureLoader.h>

namespace chai::gfx
{
    ModelRegistry::ModelRegistry(IMeshRegistry& meshes,
        ITextureRegistry& textures,
        IMaterialRegistry& materials,
        ServiceLocator* locator)
        : meshes_(meshes), textures_(textures), materials_(materials), locator_(locator),
          pool_(locator_->resolve<ITextureLoader>(), 8)
    {
    }

    ModelRegistry::~ModelRegistry()
    {

    }

    [[nodiscard]] std::shared_ptr<const ModelPrefab>
    ModelRegistry::load(AssetId id, const std::filesystem::path& path)
    {
        //dedup
        if (auto it = loaded_.find(id.value); it != loaded_.end())
            return it->second.prefab;

        //read from file
        auto bytes = readFileBytes(path);
        if (bytes.empty()) {
            CHAI_LOG_ERROR("ModelRegistry::load: could not read '{}'", path.string());
            return nullptr;
        }

        auto loader = locator_->tryResolve<IModelLoader>();
        if (!loader) {
            CHAI_LOG_ERROR("ModelRegistry::load: no IModelLoader for '{}'", path.string());
            return nullptr;
        }

        //use the loader to convert raw file bytes -> cpu data
        auto parsed = loader->decode(bytes, path.parent_path());
        if (!parsed) {
            CHAI_LOG_ERROR("ModelRegistry::load: decode failed for '{}'", path.string());
            return nullptr;
        }
        ModelAsset& model = *parsed;

        //The Registry entry we are building
        Loaded entry;
        entry.prefab = std::make_shared<ModelPrefab>();
        ModelPrefab& prefab = *entry.prefab;

        //Images come from the model as raw bites with an image index
        std::unordered_map<std::uint32_t, Handle<Texture>> imageCache;
        auto resolveImage = [&](int imageIndex, bool srgb) -> Handle<Texture> {
            if (imageIndex < 0 || imageIndex >= static_cast<int>(model.images.size()))
                return {};
            const std::uint32_t key =
                (static_cast<std::uint32_t>(imageIndex) << 1) | static_cast<std::uint32_t>(srgb);
            if (auto it = imageCache.find(key); it != imageCache.end())
                return it->second;

            //Register this texture with the texture registry
            Handle<Texture> h = ingestImage(id, model.images[imageIndex], imageIndex, srgb);
            imageCache.emplace(key, h);
            return h;
        };

        //Create material asset for each material, register textures as needed
        std::vector<Handle<Material>> matHandles(model.materials.size());
        for (std::size_t i = 0; i < model.materials.size(); ++i) {
            const ModelAsset::MaterialDesc& m = model.materials[i];

            MaterialAsset asset{};
            asset.name = m.name;
            asset.baseColorFactor = m.baseColorFactor;
            asset.metallic = m.metallic;
            asset.roughness = m.roughness;
            asset.emissiveFactor = m.emissiveFactor;
            asset.alphaMode = m.alphaMode;
            asset.doubleSided = m.doubleSided;

            asset.baseColor = resolveImage(m.baseColor, true);
            asset.emissive = resolveImage(m.emissive, true);
            asset.normal = resolveImage(m.normal, false);
            asset.metallicRoughness = resolveImage(m.metallicRoughness, false);
            asset.occlusion = resolveImage(m.occlusion, false);

            // The material now owns these texture references
            AssetId matId = subId(id, "mat:" + std::to_string(i));
            Handle<Material> h = materials_.ingest(matId, std::move(asset));
            matHandles[i] = h;
            entry.materials.push_back(h);
        }

        //Create mesh prefab. The materials have already been created
        prefab.meshGroups.reserve(model.meshes.size());
        for (std::size_t mi = 0; mi < model.meshes.size(); ++mi) {
            ModelAsset::MeshEntry& entryMesh = model.meshes[mi];

            ModelPrefab::MeshGroup group;
            group.name = entryMesh.name;
            group.primitives.reserve(entryMesh.primitives.size());

            for (std::size_t pi = 0; pi < entryMesh.primitives.size(); ++pi) {
                ModelAsset::Primitive& prim = entryMesh.primitives[pi];

                // Each primitive gets its own mesh handle
                AssetId meshId = subId(id, "mesh:" + std::to_string(mi) + ":" + std::to_string(pi));
                Handle<Mesh> mh = meshes_.ingest(meshId, std::move(prim.mesh));
                entry.meshes.push_back(mh);

                Handle<Material> mat =
                    (prim.material >= 0 && prim.material < static_cast<int>(matHandles.size()))
                        ? matHandles[prim.material]
                        : materials_.defaultMaterial();

                group.primitives.push_back(ModelPrefab::Primitive{mh, mat});
            }
            prefab.meshGroups.push_back(std::move(group));
        }

        // Hierarchy
        prefab.nodes.reserve(model.nodes.size());
        for (ModelAsset::Node& n : model.nodes) {
            ModelPrefab::Node out;
            out.position = n.position;
            out.rotation = n.rotation;
            out.scale = n.scale;
            out.meshGroup = n.meshIndex;
            out.children = std::move(n.children);
            out.name = std::move(n.name);
            prefab.nodes.push_back(std::move(out));
        }
        prefab.roots = std::move(model.roots);

        CHAI_LOG_INFO("ModelRegistry: loaded '{}' ({} groups, {} materials, {} nodes)",
                      path.string(),
                      prefab.meshGroups.size(),
                      entry.materials.size(),
                      prefab.nodes.size());

        auto [it, _] = loaded_.try_emplace(id.value, entry);
        return it->second.prefab;

    }

    void ModelRegistry::release(AssetId id)
    {
        auto it = loaded_.find(id.value);
        if (it == loaded_.end())
            return;
        releaseEntry(it->second);
        loaded_.erase(it);

    }

    void ModelRegistry::releaseAll()
    {
        for (auto& [key, entry] : loaded_)
            releaseEntry(entry);
        loaded_.clear();
    }

    void ModelRegistry::releaseEntry(Loaded& entry)
    {
        for (Handle<Mesh> h : entry.meshes)
            meshes_.release(h);
        for (Handle<Material> h : entry.materials)
            materials_.release(h);
        entry.meshes.clear();
        entry.materials.clear();
    }


    AssetId ModelRegistry::subId(AssetId base, const std::string& suffix)
    {
        return makeAssetId(std::to_string(base.value) + ":" + suffix);
    }

    Handle<Texture>
    ModelRegistry::ingestImage(AssetId modelId, const ModelAsset::ImageData& img, int idx, bool srgb)
    {
        auto loader = locator_->tryResolve<ITextureLoader>();
        if (!loader) {
            CHAI_LOG_ERROR("ModelRegistry: no ITextureLoader for embedded image {}", idx);
            return {};
        }

        TextureFormat format = srgb ? TextureFormat::RGBA8_SRGB : TextureFormat::RGBA8_UNORM;
        AssetId texId = subId(modelId, "img:" + std::to_string(idx) + (srgb ? ":srgb" : ":lin"));

        Handle<Texture> h = textures_.reserve(texId);
        pool_.enqueue({texId, img.bytes, format});

        return h;
    }

    void ModelRegistry::tick()
    {
        //collect decodes from the worker threads
        auto results = pool_.collect();
        if (!results.empty()) {
            for (const DecodeResult& r : results) {
                if (!r.jobSucceeded) {
                    CHAI_LOG_ERROR("Failed to decode image.");
                    continue;
                }
                textures_.ingest(r.id, std::move(r.asset));
            }
        }
    }
}