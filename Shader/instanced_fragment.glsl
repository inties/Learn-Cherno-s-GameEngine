#version 450 core

// 输入
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;
flat in int v_ObjectID;

// 材质属性
layout (binding = 0) uniform sampler2D u_DiffuseTexture;

// 输出
layout (location = 0) out vec4 FragColor; 
layout (location = 1) out int ObjectID;  

void main()
{
    // 基础颜色
    vec3 albedo = texture(u_DiffuseTexture, v_TexCoord).rgb;
    
    // 法线
    vec3 normal = normalize(v_Normal);
    
    // 环境光
    vec3 ambient = 0.1 * albedo;
    
    // 最终颜色
    vec3 result = ambient;
    
    FragColor = vec4(result, 1.0);
    
    // 对象ID用于拾取
    ObjectID = v_ObjectID;
}
