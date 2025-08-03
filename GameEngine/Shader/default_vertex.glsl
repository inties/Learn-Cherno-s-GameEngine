#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    // Calculate world position
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    
    // Transform normal to world space
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    
    // Pass texture coordinates
    v_TexCoord = a_TexCoord;
    
    // Final position
    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}