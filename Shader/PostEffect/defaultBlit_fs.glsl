#version 420 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
in vec2 TexCoords;

// 帧纹理
layout(binding = 0) uniform sampler2D SceneColorTex;
layout(binding = 1) uniform sampler2D TransparentAccumTex; // rgb: Σ(c*α), a: revealage
layout(binding = 2) uniform sampler2D TransparentAlphaTex; // r: Σ(α)



void main()
{
    vec3 scene = texture(SceneColorTex, TexCoords).rgb;

    vec4 transAccum = texture(TransparentAccumTex, TexCoords);
    vec3 accumColor = transAccum.rgb;
    float revealage = clamp(transAccum.a, 0.0, 1.0);
    float accumAlpha = texture(TransparentAlphaTex, TexCoords).r;

    // 透明平均色（避免除零）
    const float EPS = 1e-6;
    vec3 transparentAvgColor = (accumAlpha > EPS) ? (accumColor / accumAlpha) : vec3(0.0);

    // 合成（与原公式等价，但更稳健）
    float t = 1.0 - revealage;              // 透明混合权重
    vec3 color = mix(transparentAvgColor, scene, revealage);

    // 输出与 Bloom
    FragColor = vec4(color, 1.0);
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    BloomColor = (brightness > 1.0f) ? vec4(color, 1.0) : vec4(0.0);
}