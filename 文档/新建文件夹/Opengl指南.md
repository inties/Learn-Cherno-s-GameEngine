Opengl指南



##### opengl的顶点数据输入

用VAO描述顶点布局，并将顶点属性元素绑定到指定绑定点上，glsl中顶点属性从相应绑定点获取数据。

```
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
```



##### SSBO绑定

```
// 使用alignas(16)确保数据对齐，这对于SSBO很重要，特别是当结构体中有vec3时。
// 使用vec4和mat4通常更安全。
struct InstanceData {
    glm::mat4 modelMatrix;
    glm::vec4 color;
};


//  创建SSBO并上传数据
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, INSTANCE_COUNT * sizeof(InstanceData), &instances[0], GL_STATIC_DRAW);
    
// 将SSBO绑定到指定的绑定点 (binding = 0)
    // 这个绑定点必须与着色器中 layout(..., binding = 0) 的值匹配！
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // 解绑，以防意外修改
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    
    
    
   #core 430
    GLSL端代码：
    // 实例数据结构体，必须与C++端的内存布局匹配
// 注意：使用std430布局时，vec3和vec4的对齐方式可能需要特别注意。
// 这里为了简单和安全，我们使用vec4来存储位置和颜色。
struct InstanceData {
    mat4 modelMatrix;
    vec4 color;
};

// 关键部分：定义SSBO
// layout(std430, binding = 0) 指定了内存布局和绑定点
// readonly 是一个性能提示，表示我们只从中读取数据
layout(std430, binding = 0) readonly buffer InstanceSSBO {
    InstanceData instances[]; // 一个大小不定的实例数据数组
};

InstanceData currentInstance = instances[gl_InstanceID];
```





现代方案：

DSA、内存映射和手动管理内存屏障



##### 结构体对齐方案比较



**Dx12 hlsl端对齐方案：**

以float4即16字节为基本内存块，单个数据元素不能够跨内存块存储，但可以一个内存块存多个数据元素，不足的填充到16字节。同时单个数据元素也要满足其对齐要求



std430对齐方案：

紧密填充，不需要以16字节为基本内存块，只需要每个数据元素满足其对齐要求



std140对齐方案：
每个数据元素都必须对齐到16字节，不允许一个内存块存储多个数据元素



hlsl和std430数据元素对齐要求：

vec3和vec4都是16字节对齐，int /float等4字节对齐，double 8字节对齐，vec2是8字节对齐







**C++结构体：**

仅要求各数据元素遵循其对齐要求（即偏移量为对齐要求的倍数），不足的填充。

结构体对齐和最大元素对齐要求相同

```
// glm库内部的实现会类似这样（简化版）
//16字节对齐
struct alignas(16) vec4 {
    float x, y, z, w;
};


//4字节对齐
struct vec3 {
    float x, y, z,;
};

```





##### 纹理

现代方案使用gltexture* api，特点是DSA和更多的控制https://gemini.google.com/share/e2912f5d4ebd

###### 纹理绑定方案

传统方式激活全局的活动纹理单元，将纹理单元和纹理绑定，同时将shader变量绑定到特定的纹理单元上。即以纹理单元作为中间“介质”。

DSA（Direct State Accss）则允许不绑定对象的情况下直接修改它们的状态，纹理绑定简化为：
```
// 直接将 diffuseTextureID 绑定到 0 号纹理单元
glBindTextureUnit(0, diffuseTextureID);

// Shader (GLSL)
layout(binding = 0) uniform sampler2D u_Texture;
```

DSA方案相当于DX12中的根描述符



###### 纹理上传



###### 纹理Mipmap

```
// 假设你有一个纹理ID: textureID
// 1. 生成纹理（无需先绑定）
glGenTextures(1, &textureID);

// 2. 预先分配存储：Mipmap 级别
glTextureStorage2D(textureID, level, GL_RGBA8, 256, 256);

// 缩小使用三线性过滤
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 放大使用双线性过滤
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


// 假设 'data' 是 256x256 的原始像素数据 (Level 0)
// 上传 Level 0 的数据
glTextureSubImage2D(textureID, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, data);

为生成的数据重新生成mipmap
glGenerateTextureMipmap(m_RendererID);


```



不能使用mipmap的纹理：

渲染目标、深度缓冲等、整数纹理、MSAA纹理



##### 帧缓冲的完整性

现代opengl允许只有颜色纹理或者深度纹理附件，各纹理附件应该有相同的采样数目和尺寸。

在没有深度附件的情况下，深度测试默认通过



##### 绑定和解绑操作

opengl更换绑定前，不需要做解绑操作，调用glbind*()会直接覆盖当前的全局状态

texture绑定到纹理单元是全局操作，和shader无关，“**Shader 并不直接持有纹理对象**，它只知道要从哪个纹理单元（texture unit）取样；而纹理单元全局地绑定了一个纹理对象。”

而shader的uniform变量被shader program持有，shader切换后，uniform变量也会随之切换



GL纹理格式
对于漫反射纹理，总是使用**`GL_SRGB8_ALPHA8`**

HDR和环境贴图使用**`GL_RGBA16F`**

#### 和AI的讨论

GL的纹理绑定现代方法

https://g.co/gemini/share/fc217cde0e0c

GL绑定和解绑问题的讨论

https://chatgpt.com/share/68e93232-3f8c-8003-a8a9-69a2ed817660

后处理pass减小切换FBO开销的最佳实践

https://g.co/gemini/share/f58a49fa0124

GL纹理格式

https://chat.deepseek.com/share/q86orala4e3cfvgs3k
