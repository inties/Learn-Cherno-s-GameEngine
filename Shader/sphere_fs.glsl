#version 450 core
#define PI 3.14159265359
// 输入
in vec3 Position_ws;
in vec3 Normal_ws;
in vec2 v_TexCoord;
flat in int v_ObjectID;

// 材质属性
layout (binding = 0) uniform sampler2D u_DiffuseTexture;
layout (binding = 1) uniform sampler2D u_MetalRoughness;
layout (binding = 2) uniform sampler2D u_envMap_irradiance;
layout (binding = 3) uniform sampler2D u_envMap_radiance;
layout (binding = 4) uniform sampler2D u_brdf_lut;


uniform float roughness;

// 光照
uniform vec3 lightDir;
uniform vec3 cameraPos_ws;
// 输出
layout (location = 0) out vec4 FragColor; 
layout (location = 1) out int id; 

float GGX(float dot,float k){
    return dot/(dot*(1-k)+k);
}
float G_smith(float NdotV,float NdotL,float roughness){
    float k=(roughness+1)*(roughness+1)/8;
    return GGX(NdotV,k)*GGX(NdotL,k);
}
vec3 F_Schlick(vec3 F0, float VdotH){
    return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
}
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 Getuv(vec3 dir){
    vec2 uv=vec2(atan(dir.z, dir.x),acos(dir.y));
    uv*=invAtan;
    uv.y=1.0-uv.y;
    uv.x=uv.x+0.5;  
    return uv;
}
void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    // 首先声明和计算法线
    vec3 normal = normalize(Normal_ws);
    
    
    // 计算光照方向
    vec3 lightDir_normalized = -normalize(lightDir);
    vec3 viewDir = normalize(cameraPos_ws - Position_ws);
    vec3 H = normalize(lightDir_normalized + viewDir);
    vec3 R = reflect(-viewDir, normal);
    R=normalize(R);
    
    // 计算各种点积
    float NdotL = max(dot(normal, lightDir_normalized), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotH = max(dot(normal, H), 0.0);
    float VdotH = max(dot(viewDir, H), 0.0);   

    // 获取材质属性
    vec3 albedo = texture(u_DiffuseTexture, v_TexCoord).rgb;
    float metal = texture(u_MetalRoughness, v_TexCoord).r;
    float rough=texture(u_MetalRoughness, v_TexCoord).g;
    float roughness_2;
    roughness_2 = rough * rough;
    vec3 F0 = mix(vec3(0.004, 0.004, 0.004), albedo, metal);
    

    // 镜面反射 (Cook-Torrance BRDF)
    float f = NdotH * NdotH * (roughness_2 - 1.0) + 1.0;
    float NDF = roughness_2 / (PI * f * f);
    float G = G_smith(NdotV, NdotL, rough);
    vec3 F = F_Schlick(F0, VdotH);
    vec3 specular_brdf = NDF * G * F / max(4.0 * NdotL * NdotV, 0.001);
    
    // 漫反射
    vec3 kd=(1-F)*(1-metal);
    vec3 diffuse_brdf = albedo / PI;

    vec3 specular = specular_brdf * NdotL*lightColor;
    vec3 diffuse = kd*diffuse_brdf *NdotL*lightColor;
    
    const float MaxLod=5.0;
    vec2 lut =texture(u_brdf_lut, vec2(NdotV, rough)).xy;
    vec3 ambient_diffuse = kd*diffuse_brdf*texture(u_envMap_irradiance, Getuv(normal)).rgb;
    vec3 ambient_specular= (F0*lut.x+lut.y)*textureLod(u_envMap_radiance, Getuv(R),rough*MaxLod).rgb;
    vec3 ambient = ambient_diffuse+ambient_specular;
    vec3 color=diffuse+specular+ambient;
    // 输出最终颜色 - 显示lightDir的值用于调试
    FragColor = vec4(color, 1.0);
    
    
    // 对象ID用于拾取
    id = v_ObjectID;
}
