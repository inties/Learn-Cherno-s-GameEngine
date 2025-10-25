#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

uniform sampler2D u_Input;
layout(rgba16f, binding = 1) writeonly uniform image2D u_Output;

void main() {
    float gamma=2.2;
    float gamma_inv=1.0/gamma;
    ivec2 uv = ivec2(gl_GlobalInvocationID.xy);
    vec4 color = texelFetch(u_Input, uv, 0);
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
    color.rgb = pow(color.rgb, vec3(gamma_inv));
    imageStore(u_Output, uv, vec4(color.rgb, 1.0));
}