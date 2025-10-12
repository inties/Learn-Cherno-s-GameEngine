#version 420 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
out vec3 textureDir;


void main() {
    textureDir = a_Position;
    vec4 pos=u_ViewProjection * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

