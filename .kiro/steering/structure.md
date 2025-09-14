# GameEngine Project Structure

## Root Directory Layout
```
GameEngine/                 # Root project directory
├── GameEngine/            # Core engine library (shared DLL)
├── SandBox/              # Example application and test environment
├── vendor/               # Third-party dependencies
├── 文档/                 # Chinese documentation and design docs
├── build/                # CMake build output directory
├── bin/                  # Premake build output directory
├── bin-int/              # Intermediate build files
└── .kiro/                # Kiro IDE configuration
```

## GameEngine/ (Core Library)
```
GameEngine/
├── src/                  # Source code
│   ├── pch.h            # Precompiled header
│   ├── pch.cpp          # PCH implementation
│   └── Engine/          # Engine namespace code
│       ├── core.h       # Core engine definitions
│       ├── log.h        # Logging system
│       ├── Scene/       # ECS and scene management
│       │   ├── Entity.h/cpp      # Entity wrapper
│       │   ├── Component.h       # Component definitions
│       │   ├── Scene.h/cpp       # Scene management
│       │   └── ScriptableEntity.h # Scripting interface
│       ├── Renderer/    # Rendering system
│       │   ├── Buffer.h/cpp      # Vertex/Index buffers
│       │   ├── VertexArray.h/cpp # VAO abstraction
│       │   ├── Shader.h/cpp      # Shader management
│       │   ├── Texture.h/cpp     # Texture system
│       │   ├── Material.h/cpp    # Material system
│       │   ├── Renderer.h/cpp    # High-level renderer
│       │   ├── RenderCommand.h/cpp # Command dispatcher
│       │   └── RendererAPI.h/cpp  # API abstraction
│       └── Debug/       # Debugging utilities
│           └── Instrumentor.h    # Performance profiling
├── dependency/          # Third-party libraries
│   ├── GLFW_3_4/       # Window management
│   ├── glad/           # OpenGL loader
│   ├── imgui/          # GUI library
│   ├── glm_99/         # Mathematics library
│   ├── entt/           # ECS framework
│   ├── spdlog/         # Logging library
│   └── libs/           # Compiled libraries
├── Shader/             # Shader source files
└── CMakeLists.txt      # CMake configuration
```

## SandBox/ (Test Application)
```
SandBox/
├── src/                # Application source code
│   └── SandBoxApp.cpp  # Main application entry
├── assets/             # Runtime assets
│   ├── shaders/        # Shader files
│   ├── textures/       # Texture files
│   └── models/         # 3D model files
├── backpack/           # Example model assets
└── CMakeLists.txt      # CMake configuration
```

## Documentation Structure (文档/)
```
文档/
├── ECS架构实现指南.md        # ECS implementation guide
├── render.md               # Renderer design document
├── 渲染器架构改进方案.md      # Renderer architecture improvements
├── 模型加载系统设计.md        # Model loading system design
├── 事件系统.md              # Event system documentation
├── editor.md               # Editor design notes
├── 开发日记.md              # Development diary
└── 资源管理/               # Resource management docs
```

## Key File Patterns

### Header Organization
- **Public Headers**: Direct includes in `GameEngine/src/Engine/`
- **Implementation Headers**: Platform-specific in subdirectories
- **PCH Usage**: All engine files include `pch.h` first

### Naming Conventions
- **Classes**: PascalCase (e.g., `VertexBuffer`, `RenderCommand`)
- **Files**: Match class names (e.g., `VertexBuffer.h/.cpp`)
- **Namespaces**: `Engine` for all engine code
- **Macros**: UPPER_CASE with `ENGINE_` prefix

### Component Architecture
- **Abstract Base Classes**: Define interfaces (e.g., `VertexBuffer`)
- **Platform Implementations**: Prefixed with API name (e.g., `OpenGLVertexBuffer`)
- **Factory Methods**: Static `Create()` methods for object instantiation
- **RAII**: All resources managed with smart pointers and destructors

### Build Artifacts
- **CMake Output**: `build/bin/<Config>/`
- **Premake Output**: Root directory
- **Intermediate Files**: `bin-int/` or `build/` subdirectories
- **Dependencies**: Statically linked or copied to output directory