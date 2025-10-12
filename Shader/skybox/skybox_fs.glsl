#version 420 core

layout(binding = 0) uniform samplerCube u_skybox;
in vec3 textureDir;
out vec4 FragColor;

void main(){
    FragColor = texture(u_skybox, textureDir);
}