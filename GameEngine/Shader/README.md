# Shader Implementation for Model Loading

## Overview

This directory contains shaders designed to work with the ModelLoader system in the GameEngine.

## Files Created

### 1. `default.glsl`
A combined vertex and fragment shader that serves as the default shader for the ModelLoader system.

**Features:**
- Supports all vertex attributes expected by ModelLoader: `a_Position`, `a_Normal`, `a_TexCoord`, `a_Tangent`, `a_Bitangent`
- Handles texture uniforms as expected by ModelLoader: `u_AlbedoMap`, `u_SpecularMap`, `u_NormalMap`, etc.
- Uses `u_DiffuseColor` for material properties
- Simple directional lighting implementation
- Automatic fallback texture handling

**Shader Registration:**
- Registered as "DefaultPBR" in the ShaderLibrary (primary)
- Also registered as "Default" (fallback)

### 2. `default_vertex.glsl` & `default_fragment.glsl`
Separate vertex and fragment shader files (for reference/debugging).

## ModelLoader Integration

The ModelLoader expects specific naming conventions:

### Vertex Attributes
```glsl
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;
```

### Texture Uniforms
```glsl
uniform sampler2D u_AlbedoMap;      // Diffuse texture
uniform sampler2D u_SpecularMap;    // Specular texture
uniform sampler2D u_NormalMap;      // Normal map
uniform sampler2D u_MetallicMap;    // Metallic map
uniform sampler2D u_RoughnessMap;   // Roughness map
uniform sampler2D u_AOMap;          // Ambient occlusion map
uniform sampler2D u_Texture;       // Fallback texture
```

### Transform Uniforms
```glsl
uniform mat4 u_Model;           // Model matrix
uniform mat4 u_ViewProjection;  // View-Projection matrix
```

### Material Uniforms
```glsl
uniform vec4 u_DiffuseColor;    // Material diffuse color
```

## Usage

The shaders are automatically loaded by the RendererLayer and registered in the ShaderLibrary. The ModelLoader will find and use the "DefaultPBR" shader when processing materials.

## Model Loading

The RendererLayer has been modified to:
1. Load the default shaders into the ShaderLibrary
2. Load the backpack model from `E:/myGitRepos/myLearnOpengl/models/backpack/backpack.obj`
3. Render the model using its internal Draw() method

The model system handles all material binding and rendering automatically.