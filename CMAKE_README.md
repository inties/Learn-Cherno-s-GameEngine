# GameEngine CMake 构建系统

本项目已从 Premake 迁移到 CMake 构建系统。以下是构建和使用说明。

## 构建要求

- **Windows平台** (项目仅支持Windows)
- CMake 3.16 或更高版本
- Visual Studio 2019/2022
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

### 其他平台

**注意**: 此项目仅支持Windows平台。在其他平台上运行CMake将显示错误信息。

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

- 可执行文件: `bin/<配置>-<系统>-<架构>/<项目>/`
- 静态库: `bin/<配置>-<系统>-<架构>/<项目>/`
- SandBox 可执行文件: `bin/<配置>-<系统>-<架构>/SandBox/SandBox.exe`
- GameEngine DLL: `bin/<配置>-<系统>-<架构>/GameEngine/GameEngine.dll`

## 与 Premake 的差异

1. **输出目录**: 现在与Premake保持一致，使用 `bin/<配置>-<系统>-<架构>/<项目>/` 结构
2. **配置方式**: 使用CMake的配置系统替代Premake的filter系统
3. **依赖管理**: CMake的target-based依赖管理
4. **平台支持**: 与Premake一样，仅支持Windows平台

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

# 构建所有目标
cmake --build build --config Debug

# 安装/打包 (复制依赖库和资源文件到输出目录)
cmake --build build --target install --config Debug
```

## 安装/打包

项目配置了自动安装功能，会将以下内容复制到可执行文件目录：

- **可执行文件**: SandBox.exe, GameEngine.dll
- **依赖库**: glfw3.dll 等外部依赖
- **资源文件**: Shader/, Resources/ 目录
- **配置文件**: config.json 等

### 安装命令
```bash
# 安装到默认目录 (通常是 build/install/)
cmake --build build --target install

# 指定安装目录
cmake --build build --target install --config Debug
cmake -DCMAKE_INSTALL_PREFIX=./dist -P build/cmake_install.cmake
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