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
uniform float roughness;

// 光照
uniform vec3 lightDir;
uniform vec3 cameraPos_ws;

//是否是透明物体
uniform int transparent;

// 输出
layout (location = 0) out vec4 FragColor; 
layout (location = 1) out int id; 
layout (location = 2) out float alpha; 

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
void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    // 首先声明和计算法线
    vec3 normal = normalize(Normal_ws);
    
    // 计算光照方向
    vec3 lightDir_normalized = -normalize(lightDir);
    vec3 viewDir = normalize(cameraPos_ws - Position_ws);
    vec3 H = normalize(lightDir_normalized + viewDir);
    
    // 计算各种点积
    float NdotL = max(dot(normal, lightDir_normalized), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotH = max(dot(normal, H), 0.0);
    float VdotH = max(dot(viewDir, H), 0.0);   

    // 获取材质属性
    vec3 albedo = texture(u_DiffuseTexture, v_TexCoord).rgb;
    float metal = 0.5;

    float roughness_2;
    
    roughness_2=roughness*roughness;
    

    vec3 F0 = mix(vec3(0.004, 0.004, 0.004), albedo, metal);
    // 漫反射
    vec3 diffuse = (1.0 - metal) * albedo / PI;

    // 镜面反射 (Cook-Torrance BRDF)
    float f = NdotH * NdotH * (roughness_2 - 1.0) + 1.0;
    float NDF = roughness_2 / (PI * f * f);
    float G = G_smith(NdotV, NdotL, roughness);
    vec3 F = F_Schlick(F0, VdotH);
    vec3 specular = NDF * G * F / max(4.0 * NdotL * NdotV, 0.001);
    

    specular = specular * NdotL*lightColor;
    diffuse = diffuse *NdotL*lightColor;

    vec3 ambient = vec3(0.1) * albedo;
    vec3 color= diffuse + specular+ambient;
    // 输出最终颜色 - 显示lightDir的值用于调试
    if(bool(transparent)){
        float depth=gl_FragCoord.z;
        float m_alpha = 0.5f;

        float weight = m_alpha * max(pow((1-depth),3)*3000,0.01);
        FragColor = vec4(color*weight,1-m_alpha);
        alpha = m_alpha*weight;

    }
    else{
        FragColor = vec4(color,1.0);

    }
   
    
    
    // 对象ID用于拾取
    id = v_ObjectID;
    
}
