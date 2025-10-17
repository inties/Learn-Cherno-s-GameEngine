### shader、材质管理

材质封装shader、纹理、材质常量



后续需要考虑材质排序，减少状态切换开销





##### 纹理坐标

一个正方体顶点同时是几张不同纹理贴图的顶点，uv各不相同，因此它需要有几个uv纹理坐标。





#### 问题

```
    for (const auto& [name,pair] : m_Textures) {
        pair.first->Bind(pair.second);
    }
//为什么对？
```



RTTI运行时绑定为什么依赖父类的虚函数