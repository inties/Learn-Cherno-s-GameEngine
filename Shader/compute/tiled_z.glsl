#version 430 core

//分tile提取深度最大最小值
//采样深度贴图要从bit数据转化为uint
//深度要转化到线性深度
//min_z和max_z是共享内存，原子操作比较大小和写入

layout(binding = 0)uniform sampler2D u_depth;
layout(binding = 1,r32f) writeonly uniform image2D tiled_depth;                                              
uniform mat4 projection;

shared uint minDepthInt;
shared uint maxDepthInt;

layout(local_size_x = 16, local_size_y = 16) in;
void main() {
   	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
	}
    barrier();
    ivec2 outSize = imageSize(tiled_depth);
    if (gl_LocalInvocationID.x >= outSize.x || gl_LocalInvocationID.y >= outSize.y) return;
    ivec2 size = textureSize(u_depth, 0);
    ivec2 sampleCoord = clamp(ivec2(gl_GlobalInvocationID.xy),ivec2(0),size-ivec2(1));
    float depth = texelFetch(u_depth, sampleCoord,0).r;
    //min_z和max_z是共享内存，原子操作比较大小和写入    
    depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);
    uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
    barrier();
    float min_z=uintBitsToFloat(minDepthInt);
    float max_z=uintBitsToFloat(maxDepthInt);
    imageStore(tiled_depth,  ivec2(gl_GlobalInvocationID.xy), vec4(min_z,0,0,0));
    
}