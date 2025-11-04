#version 450 core
#define PI 3.14159265359
#define PointLightCount 1000
#define SpotLightsCount 100
#define MIN_SPOT_ANGLE 0.5
#define MAX_LIGHT_COUNT 1024
#define TILE_SIZE 16
// 输入
in vec3 Position_ws;
in vec3 Normal_ws;
in vec2 v_TexCoord;
flat in int v_ObjectID;
struct Light {
    vec3 strenth;
	float falloff_start;
	vec3 direction;
	float falloff_end;
	vec3 position;
	float spot_power;
};

struct SurfaceData {
    vec3 position;      // 表面世界坐标位置
    vec3 normal;        // 表面法线
    vec3 viewDir;       // 视线方向
    vec3 albedo;        // 材质反照率
    float metal;        // 金属度
    float rough;        // 粗糙度
    vec3 F0;           // F0参数
};

layout(std430, binding = 1) buffer LightData {
    readonly restrict Light lights[];  // 只保留一个不定长数组，并放在最后
};
layout(std430, binding = 3) buffer VisibleLights {
    readonly restrict int visible_lights[];  // 只保留一个不定长数组，并放在最后
};
// 材质属性
layout (binding = 0) uniform sampler2D u_DiffuseTexture;
layout (binding = 1) uniform sampler2D u_MetalRoughness;
layout (binding = 2) uniform sampler2D u_envMap_irradiance;
layout (binding = 3) uniform sampler2D u_envMap_radiance;
layout (binding = 4) uniform sampler2D u_brdf_lut;


uniform float roughness;
uniform vec3 direct_light_dir;
uniform vec3 direct_light_strength;
uniform vec2 screen_size;


// 光照
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

// 计算单个光源的PBR光照贡献
vec3 CalculateLightContribution(Light light, SurfaceData surface,int type) {
    // 计算光照方向
    vec3 lightDir_normalized;
    float attenuation = 1.0;
    
    if (type==0) {
        // 点光源
        vec3 lightVec = light.position - surface.position;
        float distance = length(lightVec);
        lightDir_normalized = normalize(lightVec);
        
        // 计算衰减
        if (distance > light.falloff_end) {
            return vec3(0.0); // 超出范围，无贡献
        }
        
        if (distance > light.falloff_start) {
            float attenuationFactor = (light.falloff_end - distance) / (light.falloff_end - light.falloff_start);
            attenuation = attenuationFactor * attenuationFactor;
        }
    } else if(type==1){
        // 方向光
        lightDir_normalized = -normalize(light.direction);
        attenuation = 1.0;
    }else if(type==2){
          // 点光源
        
        vec3 lightVec = light.position - surface.position;
        float distance = length(lightVec);
        lightDir_normalized = normalize(lightVec);
        float cos_theta=max(dot(light.direction, -lightDir_normalized),0.0);
        if(cos_theta<MIN_SPOT_ANGLE){
            return vec3(0.0);
        }
        // 计算衰减
        if (distance > light.falloff_end) {
            return vec3(0.0); // 超出范围，无贡献
        }
        
        if (distance > light.falloff_start) {
            float attenuationFactor = (light.falloff_end - distance) / (light.falloff_end - light.falloff_start);
            attenuation = attenuationFactor * attenuationFactor;
            attenuation = attenuation * pow(cos_theta,light.spot_power);
        }
    }
    
    vec3 H = normalize(lightDir_normalized + surface.viewDir);
    
    // 计算各种点积
    float NdotL = max(dot(surface.normal, lightDir_normalized), 0.0);
    float NdotV = max(dot(surface.normal, surface.viewDir), 0.0);
    float NdotH = max(dot(surface.normal, H), 0.0);
    float VdotH = max(dot(surface.viewDir, H), 0.0);
    
    if (NdotL <= 0.0) {
        return vec3(0.0); // 背面，无贡献
    }
    
    float roughness_2 = surface.rough * surface.rough;
    
    // 镜面反射 (Cook-Torrance BRDF)
    float f = NdotH * NdotH * (roughness_2 - 1.0) + 1.0;
    float NDF = roughness_2 / (PI * f * f);
    float G = G_smith(NdotV, NdotL, surface.rough);
    vec3 F = F_Schlick(surface.F0, VdotH);
    vec3 specular_brdf = NDF * G * F / max(4.0 * NdotL * NdotV, 0.001);
    
    // 漫反射
    vec3 kd = (1.0 - F) * (1.0 - surface.metal);
    vec3 diffuse_brdf = surface.albedo / PI;
    
    vec3 specular = specular_brdf * NdotL * light.strenth * attenuation;
    vec3 diffuse = kd * diffuse_brdf * NdotL * light.strenth * attenuation;
    
    return diffuse + specular;
}
void main()
{
    // 计算基础向量
    vec3 normal = normalize(Normal_ws);
    vec3 viewDir = normalize(cameraPos_ws - Position_ws);
    vec3 R = reflect(-viewDir, normal);
    R = normalize(R);
    
    // 获取材质属性
    vec3 albedo = texture(u_DiffuseTexture, v_TexCoord).rgb;
    float metal = texture(u_MetalRoughness, v_TexCoord).r;
    float rough = texture(u_MetalRoughness, v_TexCoord).g;
    vec3 F0 = mix(vec3(0.004, 0.004, 0.004), albedo, metal);
    
    // 创建表面数据结构体
    SurfaceData surface;
    surface.position = Position_ws;
    surface.normal = normal;
    surface.viewDir = viewDir;
    surface.albedo = albedo;
    surface.metal = metal;
    surface.rough = rough;
    surface.F0 = F0;
    
    // 计算NdotV用于环境光照
    float NdotV = max(dot(normal, viewDir), 0.0);
    
    // 初始化光照累积颜色
    vec3 totalLighting = vec3(0.0);
    
    //计算所属tile
    //计算渲染目标的分辨率
    ivec2 TileNum =ivec2((screen_size.x+TILE_SIZE-1)/TILE_SIZE, (screen_size.y+TILE_SIZE-1)/TILE_SIZE);
    ivec2 TileID=ivec2(gl_FragCoord.xy/TILE_SIZE);
    int tile_idx=TileID.x+TileID.y*TileNum.x;
    int light_offset=tile_idx*MAX_LIGHT_COUNT;
    for(int i=0;i<MAX_LIGHT_COUNT;i++){
        int id=visible_lights[light_offset+i];
        if(id==-1){
            break;
        }
        Light light = lights[id];
        if(id<PointLightCount){
            totalLighting+=CalculateLightContribution(light, surface,0);
        }else{
            totalLighting+=CalculateLightContribution(light, surface,2);
        }    
    }
    // //平行光
    // Light mainlight = Light(direct_light_strength, 0.0, direct_light_dir, 0.0, vec3(0.0), 0.0);
    // totalLighting += CalculateLightContribution(mainlight, surface, 1);
//     //遍历点光源
//     for (int i = 0; i < PointLightCount; i++) {
//        Light light = lights[i];
        
//         //计算当前光源的贡献
//        vec3 lightContribution = CalculateLightContribution(light, surface,0);
        
//        totalLighting += lightContribution;
//    }
//     //遍历 spotlights
//     for (int i = 0; i < SpotLightsCount; i++) {
//        Light light = lights[PointLightCount+i];
        
//         //计算当前光源的贡献
//        vec3 lightContribution = CalculateLightContribution(light, surface,2);
        
//        totalLighting += lightContribution;
//     }
    //添加主光源（方向光）
    Light mainlight = Light(direct_light_strength, 0.0, direct_light_dir, 0.0, vec3(0.0), 0.0);
    totalLighting += CalculateLightContribution(mainlight, surface, 1);
    //计算环境光照
    const float MaxLod = 5.0;
    vec2 lut = texture(u_brdf_lut, vec2(NdotV, rough)).xy;
    vec3 kd = (1.0 - F_Schlick(F0, NdotV)) * (1.0 - metal);
    vec3 diffuse_brdf = albedo / PI;
    
    vec3 ambient_diffuse = kd * diffuse_brdf * texture(u_envMap_irradiance, Getuv(normal)).rgb;
    vec3 ambient_specular = (F0 * lut.x + lut.y) * textureLod(u_envMap_radiance, Getuv(R), rough * MaxLod).rgb;
    vec3 ambient = ambient_diffuse + ambient_specular;
    
    // 最终颜色 = 直接光照 + 环境光照
    vec3 color = totalLighting + ambient;
    
    // 输出最终颜色
    FragColor = vec4(color, 1.0);
    
    // 对象ID用于拾取
    id = v_ObjectID;
}
