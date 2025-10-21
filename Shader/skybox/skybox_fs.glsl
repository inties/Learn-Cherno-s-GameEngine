#version 420 core
#define pi 3.14159265359
//layout(binding = 0) uniform samplerCube environmentMap;
layout(binding = 0) uniform sampler2D environmentMap;

in vec3 localPosition;
out vec4 FragColor;
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 Getuv(vec3 dir){
    vec2 uv=vec2(atan(dir.z, dir.x),acos(dir.y));
    uv*=invAtan;
    uv.y=1.0-uv.y;
    uv.x=uv.x+0.5;  
    return uv;
}
void main(){

    vec2 uv=Getuv(normalize(localPosition)).xy;
    FragColor = texture(environmentMap, uv);
}