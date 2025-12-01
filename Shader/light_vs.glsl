#version 450 core

layout(location = 0) in vec3 a_Position;
out vec3 color;

// 视图投影矩阵（每批次设置一次）
uniform mat4 u_ViewProjection;

struct InstanceData {
    vec3 color;
	float falloff_start;
	vec3 direction;
	float falloff_end;
	vec3 position;
	float spot_power;
};

layout(std430, binding = 2) buffer Instances {
    readonly restrict InstanceData instances[];  // 只保留一个不定长数组，并放在最后
};



void main()
{
    // 获取当前实例的索引
    int instanceID = gl_InstanceID;
    
    // 求出在裁剪空间中的坐标
    vec4 worldPosition = vec4(a_Position * 0.1, 1.0) + vec4(instances[instanceID].position, 0.0);
    gl_Position = u_ViewProjection * worldPosition;
    color = instances[instanceID].color;
    
}