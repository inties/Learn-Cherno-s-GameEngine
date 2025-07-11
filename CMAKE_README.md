# GameEngine CMake 构建系统

本项目已从 Premake 迁移到 CMake 构建系统。以下是构建和使用说明。

## 构建要求

- CMake 3.16 或更高版本
- Visual Studio 2019/2022 (Windows)
- C++17 支持的编译器

## 快速开始

### Windows (推荐)

1. **使用提供的构建脚本:**
   ```batch
   build.bat
   ```

2. **手动构建:**
   ```batch
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Debug
   ```

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## 项目结构

```
GameEngine/
├── CMakeLists.txt                    # 顶级CMake配置
├── GameEngine/
│   ├── CMakeLists.txt               # GameEngine共享库
│   └── dependency/
│       ├── glad/
│       │   └── CMakeLists.txt       # Glad静态库
│       └── imgui/
│           └── CMakeLists.txt       # ImGui静态库
└── SandBox/
    └── CMakeLists.txt               # SandBox应用程序
```

## 构建配置

支持以下构建配置：
- **Debug**: 调试版本，包含调试符号
- **Release**: 发布版本，开启优化
- **RelWithDebInfo**: 发布版本，包含调试信息
- **MinSizeRel**: 最小尺寸发布版本

## 输出目录

- 可执行文件: `build/bin/<配置>/`
- 静态库: `build/lib/<配置>/`
- SandBox 可执行文件: `build/bin/<配置>/SandBox.exe`

## 与 Premake 的差异

1. **输出目录**: CMake使用 `build/` 目录而不是项目根目录
2. **配置方式**: 使用CMake的配置系统替代Premake的filter系统
3. **依赖管理**: CMake的target-based依赖管理
4. **跨平台**: 更好的跨平台支持

## IDE 支持

### Visual Studio
```batch
cd build
start GameEngine.sln
```

### VS Code
安装 CMake Tools 扩展，然后：
1. 打开项目文件夹
2. Ctrl+Shift+P → "CMake: Configure"
3. 选择编译器和构建类型

## 常用命令

```bash
# 重新配置
cmake --build build --target clean
cmake -B build

# 仅构建特定目标
cmake --build build --target GameEngine
cmake --build build --target SandBox

# 安装 (如果配置了安装目标)
cmake --build build --target install
```

## 故障排除

1. **找不到库文件**: 确保所有依赖库在正确的目录中
2. **编译错误**: 检查C++17支持和编译器版本
3. **链接错误**: 验证库文件路径和名称

## 从 Premake 迁移

如果您之前使用Premake构建：
1. 清理之前的构建文件
2. 使用新的CMake构建系统
3. 更新IDE项目配置

原Premake文件保留在项目中，可以继续使用 `generateproj.bat` 生成Visual Studio项目。 