#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

// 视图投影矩阵（每批次设置一次）
uniform mat4 u_ViewProjection;

struct InstanceData {
    mat4 model;
    vec4 extra;
};

layout(std430, binding = 2) buffer Instances {
    readonly restrict InstanceData instances[];  // 只保留一个不定长数组，并放在最后
};

// 输出到片段着色器
out vec3 Position_ws;
out vec3 Normal_ws;
out vec2 v_TexCoord;
flat out int v_ObjectID;

void main()
{
    // 获取当前实例的索引
    int instanceID = gl_InstanceID;
    
    // 从SSBO读取实例数据（现在使用固定大小数组）
    mat4 modelMatrix = instances[instanceID].model;
    vec4 extraData   = instances[instanceID].extra;
    
    // 提取对象ID
    v_ObjectID = int(extraData.x + 0.5);
    
    // 变换顶点位置
    vec4 worldPosition = modelMatrix * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * worldPosition;
    
    // 变换法线
    Normal_ws = mat3(transpose(inverse(modelMatrix))) * a_Normal.xyz;
    
    // 传递纹理坐标
    v_TexCoord = a_TexCoord;
    
    // 传递世界空间位置
    Position_ws = worldPosition.xyz;
}
