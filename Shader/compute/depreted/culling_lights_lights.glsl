#使用frustum面剔除光源
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
layout(binding = 1,r32f) writeonly uniform image2D debug_num;      
layout(std430, binding = 0) buffer LightData {
    readonly restrict Light lights[];  // 只保留一个不定长数组，并放在最后
}; 
layout(std430, binding = 1) buffer VisibleLight {
    writeonly restrict int visible_lights[];  // 只保留一个不定长数组，并放在最后
};                   
uniform mat4 view;                      
uniform mat4 projection;
uniform mat4 viewProjection;
uniform mat4 inv_VP_matrix;
uniform int points_light_count;
uniform int spot_light_count;
uniform float max_visible_distance;//大于这个距离的光源直接剔除



shared uint minDepthInt;
shared uint maxDepthInt;
shared vec4 frustumPlanes[6];
shared uint light_group_count;
shared uint tile_visible_light_count;
shared uint tile_visible_light[MAX_LIGHT_COUNT];   

bool cullPointsLight(vec3 position,float falloff_end);
bool cullSpotLight(vec3 position,vec3 direction,float falloff_end);

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
    depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);

    //记录最小和最大原始深度
    uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
    barrier();
       
    //计算tile的frustum面
    if(gl_LocalInvocationIndex==0){
        float min_z=uintBitsToFloat(minDepthInt);
        float max_z=uintBitsToFloat(maxDepthInt);
        min_z=min(min_z,max_visible_distance);
        max_z=min(max_z,max_visible_distance);
        ivec2 tileID = ivec2(gl_WorkGroupID.xy);
        ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
        // Convert the min and max across the entire tile back to float


		// Steps based on tile sale
		vec2 negativeStep = (2.0 * vec2(tileID)) / vec2(tileNumber);
		vec2 positiveStep = (2.0 * vec2(tileID + ivec2(1, 1))) / vec2(tileNumber);

		// Set up starting values for planes using steps and min and max z values
		frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
		frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
		frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
		frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
		frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -min_z); // Near
		frustumPlanes[5] = vec4(0.0, 0.0, 1.0, max_z); // Far
        	// Transform the first four planes
		for (uint i = 0; i < 4; i++) {
			frustumPlanes[i] *= viewProjection;
			frustumPlanes[i] /= length(frustumPlanes[i].xyz);
		}
        // Transform the depth planes
		frustumPlanes[4] *= view;
		frustumPlanes[4] /= length(frustumPlanes[4].xyz);
		frustumPlanes[5] *= view;
		frustumPlanes[5] /= length(frustumPlanes[5].xyz);
    }
    barrier();
    uint light_count;
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
        vec4 position=vec4(lights[idx].position,1.0f);
        float radius=lights[idx].falloff_end;
        float distance;
        for(int j=0;j<6;j++){
            distance=dot(position,frustumPlanes[j])+radius;
            if(distance<0.0f){
                break;
            }
        }
        if(distance>=0.0f){
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
    imageStore(debug_num,ivec2(gl_GlobalInvocationID.xy),vec4(tile_visible_light_count));
}




    


