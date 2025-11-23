# ☕ Chai Engine

**Chai** is a modular C++ game engine built from scratch for experimenting with traversal mechanics and non-Euclidean
level design.

---

## ✨ Overview

Chai is a playground for exploring engine architecture and gameplay systems. It's written in modern C++, with a focus on
clean structure, rendering abstraction, and extensibility.

Currently only Windows is supported

---

## 🛠️ Building the Code

### Prerequisites

Make sure you have the following:

- **CMake**
- **C++ Compiler**
    - MSVC (Windows)
- **Visual Studio**
- FMOD (https://www.fmod.com/)

`FMOD_HOME` must also be set in your environment for CMake to locate it. For Windows, it is likely at the following
path:
`C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows`

### Clone the Repository

```bash
git clone https://github.com/debbie-liknes/ChaiEngine.git
cd ChaiEngine

git submodule update --init
```

### Generate .sln

```bash
mkdir build && cd build
cmake ..
start Chai.sln
```

---

## 🧱 Planned Features

- 🔧 **Abstract Rendering Layer**  
  Decouples rendering from logic; supports multiple backends (OpenGL now, Vulkan in progress)

- 🌀 **Coordinate System Abstraction**  
  (In planning) Enables traversal and rendering in spherical, hyperbolic, or Euclidean space

- 🧠 **Modular Engine Architecture**  
  Systems like input, events, and scene management are built to be extensible and loosely coupled

- 📐 **Custom Math Layer**  
  3D math utilities with future support for non-standard geometry

---

## 🔜 Roadmap

---

## ⚙️ Tech Stack

- **Language:** C++
- **Rendering:** OpenGL
- **Windowing:** GLFW
- **Build:** CMake
- **Utilities:** Custom math & event libraries

---

## 📎 Notes

---

## 📚 Libraries

GLFW
glm
stb_image
glad
FMOD