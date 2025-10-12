// Debug Shader - 用于排查渲染问题
#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Color;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    // 计算世界坐标
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    // 传递法线和纹理坐标
    v_Normal = normalize(mat3(u_Model) * a_Normal);
    v_TexCoord = a_TexCoord;
    
    // 基于位置生成颜色（调试用）
    v_Color = abs(a_Position) * 0.5 + 0.5;
    
    // 最终位置
    gl_Position = u_ViewProjection * worldPos;
    
    // 调试：将深度值映射到颜色
    // v_Color = vec3(gl_Position.z / gl_Position.w * 0.5 + 0.5);
}

#type fragment
#version 330 core

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_Color;

out vec4 FragColor;

uniform sampler2D u_AlbedoMap;
uniform vec4 u_DiffuseColor;

// 调试模式开关
uniform int u_DebugMode; // 0=正常, 1=位置颜色, 2=法线颜色, 3=纹理坐标, 4=纯色

void main()
{
    vec4 result;
    
    if (u_DebugMode == 1) {
        // 显示基于位置的颜色
        result = vec4(v_Color, 1.0);
    }
    else if (u_DebugMode == 2) {
        // 显示法线作为颜色
        result = vec4(abs(v_Normal), 1.0);
    }
    else if (u_DebugMode == 3) {
        // 显示纹理坐标
        result = vec4(v_TexCoord, 0.0, 1.0);
    }
    else if (u_DebugMode == 4) {
        // 显示纯红色（确保顶点被处理）
        result = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else {
        // 正常模式：尝试纹理采样
        vec4 texColor = texture(u_AlbedoMap, v_TexCoord);
        if (length(texColor.rgb) < 0.1) {
            texColor = u_DiffuseColor;
        }
        if (length(texColor.rgb) < 0.1) {
            texColor = vec4(0.8, 0.8, 0.8, 1.0);
        }
        
        // 简单光照
        vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
        float NdotL = max(dot(v_Normal, lightDir), 0.0);
        result = vec4(texColor.rgb * (0.3 + 0.7 * NdotL), texColor.a);
    }
    
    FragColor = result;
}