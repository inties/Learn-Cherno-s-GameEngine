// Vertex Shader
#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;


out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    // Calculate world position
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    
    // Transform normal to world space
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    
    // Pass texture coordinates
    v_TexCoord = a_TexCoord;
    
    // Final position
    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}

// Fragment Shader
#type fragment
#version 330 core

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_FragPos;
layout (location = 0) out vec4 FragColor;  // 第一个渲染目标
layout (location = 1) out int id;          // 第二个渲染目标
// Texture uniforms (as expected by ModelLoader)
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_Texture; // Fallback texture

// Material properties
uniform vec4 u_DiffuseColor;

void main()
{
    // Sample diffuse texture (use u_AlbedoMap if available)
    vec4 albedo = texture(u_AlbedoMap, v_TexCoord);
    
    // If albedo is mostly black/transparent, try fallback texture
    if (length(albedo.rgb) < 0.1 || albedo.a < 0.1) {
        albedo = texture(u_Texture, v_TexCoord);
    }
    
    // If still no valid texture, use material color
    if (length(albedo.rgb) < 0.1 || albedo.a < 0.1) {
        albedo = u_DiffuseColor;
    }
    
    // Ensure we have a valid color
    if (length(albedo.rgb) < 0.1) {
        albedo = vec4(0.8, 0.8, 0.8, 1.0); // Default gray
    }
    
    // Simple lighting calculation
    vec3 norm = normalize(v_Normal);
    
    // Ambient lighting
    vec3 ambient = 0.3 * albedo.rgb;
    
    // Diffuse lighting (simple directional light)
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // Simple directional light
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * albedo.rgb;
    
    // Combine lighting
    vec3 result = ambient + diffuse;
    result=vec3(1.0,0.0,0.0);
    FragColor = vec4(result, albedo.a);
    id=10;
}