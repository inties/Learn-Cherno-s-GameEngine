#version 420 core
#define pi 3.14159265359
//layout(binding = 0) uniform samplerCube environmentMap;
layout(binding = 0) uniform sampler2D environmentMap;

in vec3 localPosition;
out vec4 FragColor;

vec2 Getuv(vec3 dir){
    return vec2(atan(dir.x, dir.z)/(2*pi)+0.5, acos(dir.y)/pi);
}
void main(){
    vec2 uv=Getuv(localPosition).xy;
    FragColor = texture(environmentMap, uv);
}