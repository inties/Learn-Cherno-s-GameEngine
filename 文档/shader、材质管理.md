### shader、材质管理

材质封装shader、纹理、材质常量



后续需要考虑材质排序，减少状态切换开销





##### 纹理坐标

一个正方体顶点同时是几张不同纹理贴图的顶点，uv各不相同，因此它需要有几个uv纹理坐标。



##### 材质常量绑定



GL的方案都是在draw call时绑定Uniform变量，但这意味着调用drawcall时，需要逐个访问这些变量的值。如果需要进行大量的绘制调用，理清每个绘制调用需要更新哪些变量是很麻烦的。

更好的方法是drawcall应该只绑定常量描述符，常量的更新和drawcall解耦。GL中类似的机制？





#### 问题

```
    for (const auto& [name,pair] : m_Textures) {
        pair.first->Bind(pair.second);
    }
//为什么对？
```



RTTI运行时绑定为什么依赖父类的虚函数