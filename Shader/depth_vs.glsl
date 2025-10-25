#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

struct InstanceData {
    mat4 model;
    vec4 extra;
};
layout(std430, binding = 0) buffer Instances {
    readonly restrict InstanceData instances[];  // 只保留一个不定长数组，并放在最后
};

uniform mat4 u_ViewProjection;

void main() {
    mat4 modelMatrix = instances[gl_InstanceID].model;
    vec4 worldPos = modelMatrix * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * worldPos;
}
