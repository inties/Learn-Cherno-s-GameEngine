Opengl指南



opengl的顶点数据输入：

用VAO描述顶点布局，并将顶点属性元素绑定到指定绑定点上，glsl中顶点属性从相应绑定点获取数据。

```
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
```







纹理绑定方案：

传统方式激活全局的活动纹理单元，将纹理单元和纹理绑定，同时将shader变量绑定到特定的纹理单元上。即以纹理单元作为中间“介质”。

DSA（Direct State Accss）则允许不绑定对象的情况下直接修改它们的状态，纹理绑定简化为：
```
// 直接将 diffuseTextureID 绑定到 0 号纹理单元
glBindTextureUnit(0, diffuseTextureID);

// Shader (GLSL)
layout(binding = 0) uniform sampler2D u_Texture;
```

DSA方案相当于DX12中的根描述符





帧缓冲的完整性：
现代opengl允许只有颜色纹理或者深度纹理附件，各纹理附件应该有相同的采样数目和尺寸。

在没有深度附件的情况下，深度测试默认通过



opengl更换绑定前，不需要做解绑操作，调用glbind*()会直接覆盖当前的全局状态

texture绑定到纹理单元是全局操作，和shader无关，“**Shader 并不直接持有纹理对象**，它只知道要从哪个纹理单元（texture unit）取样；而纹理单元全局地绑定了一个纹理对象。”

而shader的uniform变量被shader program持有，shader切换后，uniform变量也会随之切换

