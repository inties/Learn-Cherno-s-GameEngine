//使用视锥体角点进行光源剔除，弃用
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
layout(binding = 1,rgba16f) writeonly uniform image2D debug_num;      
layout(std430, binding = 0) buffer LightData {
    readonly restrict Light lights[];  // 只保留一个不定长数组，并放在最后
}; 
layout(std430, binding = 1) buffer VisibleLight {
    writeonly restrict int visible_lights[];  // 只保留一个不定长数组，并放在最后
};                                         
uniform mat4 projection;
uniform mat4 inv_VP_matrix;
uniform int points_light_count;
uniform int spot_light_count;


shared uint minDepthInt;
shared uint maxDepthInt;
shared float min_z;
shared float max_z;
shared vec3 tile_frustum[8];
shared vec4 tile_frustum_plane[6];
shared uint light_group_count;
shared uint light_count;
shared uint tile_visible_light_count;
shared uint tile_visible_light[MAX_LIGHT_COUNT];   

bool cullPointsLight(vec3 position,float falloff_end);
bool cullSpotLight(vec3 position,vec3 direction,float falloff_end);
bool cullPointsLight2(vec3 position,float falloff_end);
layout(local_size_x = TILESIZE, local_size_y = TILESIZE) in;
void main() {
   	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
	}
    barrier();
    //ivec2 outSize = imageSize(debug_num);
    //if (gl_GlobalInvocationID.x >= outSize.x || gl_GlobalInvocationID.y >= outSize.y) return;
    ivec2 size = textureSize(u_depth, 0);
    ivec2 sampleCoord = clamp(ivec2(gl_GlobalInvocationID.xy),ivec2(0),size-ivec2(1));
    float depth = texelFetch(u_depth, sampleCoord,0).r;
    //min_z和max_z是共享内存，原子操作比较大小和写入  
    //计算线性深度  
    //使用线性深度后，不再出现tile之间剔除结果差异巨大的问题
    //距离远的光源自动被剔除，无法理解
    //转为线性深度
    depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);

    //记录最小和最大原始深度
    uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
    barrier();
    
    //计算tile的frustum顶点
    if(gl_LocalInvocationIndex==0){
        min_z=uintBitsToFloat(minDepthInt);
        max_z=uintBitsToFloat(maxDepthInt);
        // min_z=0.5*projection[3][2]/min_z-0.5*projection[2][2]+0.5;
        // max_z=0.5*projection[3][2]/max_z-0.5*projection[2][2]+0.5;
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

        tile_frustum_plane[0]=vec4(0.0,0.0,-1.0,0.0);
        tile_frustum_plane[1]=vec4(-1.0,0.0,0.0,0.0);
        tile_frustum_plane[2]=vec4(0.0,1.0,0.0,0.0);
        tile_frustum_plane[3]=vec4(1.0,0.0,0.0,0.0);
        tile_frustum_plane[4]=vec4(0.0,-1.0,0.0,0.0);
        tile_frustum_plane[5]=vec4(0.0,0.0,1.0,0.0);

        tile_frustum_plane[0].w=-dot(tile_frustum_plane[0].xyz,tile_frustum[0]);
        tile_frustum_plane[1].w=-dot(tile_frustum_plane[1].xyz,tile_frustum[0]);
        tile_frustum_plane[2].w=-dot(tile_frustum_plane[2].xyz,tile_frustum[0]);
        tile_frustum_plane[3].w=-dot(tile_frustum_plane[3].xyz,tile_frustum[7]);
        tile_frustum_plane[4].w=-dot(tile_frustum_plane[4].xyz,tile_frustum[7]);
        tile_frustum_plane[5].w=-dot(tile_frustum_plane[5].xyz,tile_frustum[7]);

        //使用逆转置矩阵变换？？？，正确
        mat4 T=inverse(inv_VP_matrix);
        T=transpose(T);
        for(int i=0;i<6;i++){

            tile_frustum_plane[i]=T*tile_frustum_plane[i];
            float invLen = inversesqrt(dot(tile_frustum_plane[i].xyz, tile_frustum_plane[i].xyz));
            tile_frustum_plane[i] *= invLen;
        }
        //从屏幕空间到世界空间
        for(int i=0;i<8;i++){
            vec3 ndc=2*tile_frustum[i]-vec3(1.0);
            vec4 world=inv_VP_matrix*vec4(ndc,1.0);
            tile_frustum[i]=world.xyz/world.w;
        }

    }
    barrier();
    
    //光源剔除，每256个光源为一组
    if(gl_LocalInvocationIndex==0){
        uint thread_count=TILESIZE*TILESIZE;
        light_count=uint(points_light_count)+uint(spot_light_count);
        light_count=min(light_count,MAX_LIGHT_COUNT);
        light_group_count=(light_count+thread_count-1)/thread_count;
        tile_visible_light_count=0;
    }
    barrier();
    for(int i=0;i<light_group_count;i++){
        uint idx=i*TILESIZE*TILESIZE+gl_LocalInvocationIndex;
        if(idx>=light_count)
            break;
        // if(idx<points_light_count){
        //    if(cullPointsLight2(lights[idx].position,lights[idx].falloff_end)){
        //       uint visible_idx=atomicAdd(tile_visible_light_count,1);
        //       tile_visible_light[visible_idx]=idx;
        //    }
        // }
        // else{
        //     if(cullSpotLight(lights[idx].position,lights[idx].direction,lights[idx].falloff_end)){
        //       uint visible_idx=atomicAdd(tile_visible_light_count,1);
        //       tile_visible_light[visible_idx]=idx;
        //    }
        // }
        if(cullPointsLight2(lights[idx].position,lights[idx].falloff_end)){
                uint visible_idx=atomicAdd(tile_visible_light_count,1);
              tile_visible_light[visible_idx]=idx;
        }
    }
    //保证组内光源剔除完成，接下来将剔除结果输出到SSBO中
    barrier();
    if(gl_LocalInvocationIndex==0){
        //不知道其它组的剔除情况，应该放在SSBO中的什么位置？
        uint index=gl_WorkGroupID.y*gl_NumWorkGroups.x+gl_WorkGroupID.x;
        uint offset=index*MAX_LIGHT_COUNT;
        for(int i=0;i<tile_visible_light_count;i++){
            visible_lights[offset++]=int(tile_visible_light[i]);
        }
        if(tile_visible_light_count!=MAX_LIGHT_COUNT){
            visible_lights[offset]=-1;
        }
    }
    barrier();
    imageStore(debug_num,ivec2(gl_GlobalInvocationID.xy),vec4(min_z,max_z,tile_visible_light_count,0.0f));



    
}
float dist2(const vec3 a,const vec3 b){
    return dot(a-b,a-b);
}
bool cullPointsLight(vec3 position,float falloff_end){ 
    for(int i=0;i<8;i++){
        if(dist2(tile_frustum[i],position)<falloff_end*falloff_end)
        return true;
    }
    return false;
}
bool cullPointsLight2(vec3 position,float falloff_end){ 
    for(int i=0;i<6;i++){
        if(dot(tile_frustum_plane[i],vec4(position,1.0f))>falloff_end)   
        return false;     
    }
    return true;
}
bool cullSpotLight(vec3 position,vec3 direction,float falloff_end){ 
    for(int i=0;i<8;i++){
        if(dist2(tile_frustum[i],position)<falloff_end*falloff_end&&dot(tile_frustum[i]-position,direction)>0.5)
        return true;
    }
    return false;
}

