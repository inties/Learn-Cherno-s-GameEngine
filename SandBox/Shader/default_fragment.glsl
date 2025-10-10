#version 330 core

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_FragPos;

out vec4 FragColor;

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

// Simple lighting
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;

void main()
{
    // Sample diffuse texture (use u_AlbedoMap if available, fallback to u_Texture)
    vec4 albedo;
    
    // Try to sample from main albedo texture
    albedo = texture(u_AlbedoMap, v_TexCoord);
    
    // If albedo is mostly black/transparent, try fallback texture
    if (albedo.rgb == vec3(0.0) || albedo.a < 0.1) {
        albedo = texture(u_Texture, v_TexCoord);
    }
    
    // If still no valid texture, use material color
    if (albedo.rgb == vec3(0.0) || albedo.a < 0.1) {
        albedo = u_DiffuseColor;
    }
    
    // Ensure we have a valid color
    if (albedo.rgb == vec3(0.0)) {
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
    


    FragColor = vec4(result, albedo.a);
}