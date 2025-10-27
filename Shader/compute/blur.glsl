#version 450 core
layout(binding =0) uniform sampler2D u_Texture;

layout(rgba16f, binding = 0) writeonly uniform image2D Output;

float w[5];
uniform int vertical;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
    w[0]=0.1216;
    w[1]=0.233;
    w[2]=0.2907;
    w[3]=0.233;
    w[4]=0.1216;
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

    // 写入边界保护（避免 dispatch 尺寸大于图像尺寸时越界）
    ivec2 outSize = imageSize(Output);
    if (gid.x >= outSize.x || gid.y >= outSize.y) return;


    ivec2 offset=ivec2(1-vertical,vertical);
    vec3 color = vec3(0.0);
    ivec2 size = textureSize(u_Texture, 0);
    for(int i = -2; i <= 2; i++){        
        ivec2 sampleCoord = clamp(ivec2(gl_GlobalInvocationID.xy) + offset * i,ivec2(0),size-ivec2(1));
        vec3 s = texelFetch(u_Texture, sampleCoord,0).rgb;
        color += s * w[i + 2];
    }
    imageStore(Output, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0));

  
}