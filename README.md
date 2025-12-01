# Chai Engine

A modular C++ game engine focused on modern rendering techniques and clean architecture.

## Features

**Rendering**
- Deferred shading pipeline with configurable G-buffer
- Physically-based rendering (Cook-Torrance BRDF)
- Multi-light forward rendering support
- Skybox rendering with cubemap support

**Asset Pipeline**
- glTF 2.0 loader (tested with Intel Sponza)
- Three-tier material system (MaterialAsset → MaterialResource → MaterialInstance)
- Shader permutation system

**Architecture**
- Scene hierarchy with GameObjects and components
- Decoupled rendering backend (OpenGL; Vulkan planned)
- Event-driven input system

## Screenshots
![sponza-deferred-shading](assets/screenshots/sponza_deferred.jpg)

## Building

### Prerequisites
- CMake
- MSVC (Windows only currently)
- Visual Studio
- [FMOD](https://www.fmod.com/) with `FMOD_HOME` environment variable set

### Steps
```bash
git clone https://github.com/debbie-liknes/ChaiEngine.git
cd ChaiEngine
git submodule update --init
mkdir build && cd build
cmake ..
start Chai.sln
```

## Roadmap
- Vulkan backend
- Screen-space ambient occlusion
- Coordinate system abstraction for non-Euclidean spaces

## Dependencies
GLFW, stb_image, glad, FMOD