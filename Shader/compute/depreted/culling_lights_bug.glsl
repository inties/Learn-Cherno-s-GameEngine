#version 430 core


//分tile提取深度最大最小值
//采样深度贴图要从bit数据转化为uint
//深度要转化到线性深度
//min_z和max_z是共享内存，原子操作比较大小和写入

#define TILESIZE 16
#define MAX_LIGHT_COUNT 1024
struct Light {
    vec3 strenth;
	float falloff_start;
	vec3 direction;
	float falloff_end;
	vec3 position;
	float spot_power;
};
layout(binding = 0)uniform sampler2D u_depth;
layout(binding = 1,r32f) writeonly uniform image2D tiled_depth;      
layout(std430, binding = 0) buffer LightData {
    readonly restrict Light lights[];  // 只保留一个不定长数组，并放在最后
}; 
layout(std430, binding = 1) buffer VisibleLight {
    writeonly restrict int visible_lights[];  // 只保留一个不定长数组，并放在最后
};                                         
uniform mat4 projection;
uniform mat4 inv_projection;
uniform uint points_light_count;
uniform uint spot_light_count;



shared uint minDepthInt;
shared uint maxDepthInt;
shared vec3 tile_frustum[8];
shared uint light_group_count;
shared uint tile_visible_light_count;
shared uint tile_visible_light[MAX_LIGHT_COUNT];   

bbox cullPointsLight(Light light);
bool cullSpotLightLight(Light light);

layout(local_size_x = TILESIZE, local_size_y = TILESIZE) in;
void main() {
   	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
	}
    barrier();
    ivec2 outSize = imageSize(tiled_depth);
    if (gl_GlobalInvocationID.x >= outSize.x || gl_GlobalInvocationID.y >= outSize.y) return;
    ivec2 size = textureSize(u_depth, 0);
    ivec2 sampleCoord = clamp(ivec2(gl_GlobalInvocationID.xy),ivec2(0),size-ivec2(1));
    float depth = texelFetch(u_depth, sampleCoord,0).r;
    //min_z和max_z是共享内存，原子操作比较大小和写入    
    //depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);

    //记录最小和最大原始深度
    uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
    barrier();
    
    //计算tile的frustum顶点
    if(gl_LocalInvocationIndex==0){
        float min_z=uintBitsToFloat(minDepthInt);
        float max_z=uintBitsToFloat(maxDepthInt);
        vec2 tileID= vec2(gl_WorkGroupID);
        vec2 tileNum=vec2(gl_NumWorkGroups);
        tile_frustum[0]=vec3(tileID/tileNum,min_z);  
        tile_frustum[1]=vec3((tileID+vec2(1.0,0.0))/tileNum,min_z);
        tile_frustum[2]=vec3((tileID+vec2(0.0,1.0))/tileNum,min_z);
        tile_frustum[3]=vec3((tileID+vec2(1.0,1.0))/tileNum,min_z);
        tile_frustum[4]=vec3(tileID/tileNum,max_z);
        tile_frustum[5]=vec3((tileID+vec2(1.0,0.0))/tileNum,max_z);
        tile_frustum[6]=vec3((tileID+vec2(0.0,1.0))/tileNum,max_z);
        tile_frustum[7]=vec3((tileID+vec2(1.0,1.0))/tileNum,max_z);
        for(int i=0;i<8;i++){
            tile_frustum[i]=(inv_projection*vec4(tile_frustum[i],1.0)).xyz/tile_frustum[i].z;
        }

    }
    barrier();

    //光源剔除，每256个光源为一组
    if(gl_LocalInvocationIndex==0){
        uint thread_count=TILESIZE*TILESIZE;
        uint light_count=points_light_count+spot_light_count;
        light_count=max(light_count,MAX_LIGHT_COUNT);
        light_group_count=(light_count+thread_count-1)/thread_count;
        tile_visible_light_count=0;
    }
    barrier();
    for(int i=0;i<light_group_count;i++){
        int idx=i*TILESIZE*TILESIZE+gl_LocalInvocationIndex;
        if(idx>=light_count)
            break;
        if(idx<points_light_count){
           if(cullPointsLight(lights[idx])){
              uint visible_idx=atomicAdd(tile_visible_light_count,1);
              tile_visible_light[visible_idx]=idx;
           }
            
        }
        else{
            if(cullSpotLight(lights[idx])){
              uint visible_idx=atomicAdd(tile_visible_light_count,1);
              tile_visible_light[visible_idx]=idx;
           }
        }
    }
    //保证组内光源剔除完成，接下来将剔除结果输出到SSBO中
    barrier();
    if(gl_LocalInvocationIndex==0){
        //不知道其它组的剔除情况，应该放在SSBO中的什么位置？
        uint index=gl_WorkGroupID.y*gl_NumWorkGroups.x+gl_WorkGroupID.x;
        uint offset=index*MAX_LIGHT_COUNT;
        for(int i=0;i<tile_visible_light_count;i++){
            visible_lights[offset++]=tile_visible_light[i];
        }
        if(tile_visible_light_count!=MAX_LIGHT_COUNT){
            visible_lights[offset]=-1;
        }
    }


    
}
float distance(vec3 a,vec3 b){
    return dot(a-b,a-b);
}
bool cullPointsLight(Light light){ 
    for(int i=0;i<8;i++){
        if(distance(tile_frustum[i],light.position)<light.falloff_end*light.falloff_end)
        return true;
    }
    return false;
}
bool cullSpotLight(Light light){ 
    for(int i=0;i<8;i++){
        if(distance(tile_frustum[i],light.position)<light.falloff_end*light.falloff_end&&dot(tile_frustum[i]-light.position,light.direction)>0.5)
        return true;
    }
    return false;
}