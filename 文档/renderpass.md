

renderpipeline管理各个pass的渲染流程、依赖关系和初始化顺序



pass：集成自passbase基类

初始化：描述符设置、shader编译、初始化全局资源

资源准备：准备visible objects，考虑合批

同步等待：等待存在依赖关系资源的barrier/将barrier直接作为渲染命令添加到命令队列中

绘制命令：绘制命令立即返回，各pass之间并不同步，每一帧开始时CPU可能会同步等待GPU。



帧资源：

全局资源，命令列表等

shader可共用UBO资源，通过偏移索引访问（类似于描述符和缓冲区的关系）



renderpipeline

renderpass



资源(包括shader)不由pass管理，renderpass可注入VAO和matmanager，根据名称获取相应资源。

相关资源包括：

渲染目标：

顶点缓冲：各个pass，例如阴影pass和gbuffer pass都使用所有全局的VAO(每个shader只能绑定一个VAO)

shader:

常量缓冲：使用Uniform变量,设置pass的shader常量

纹理：不考虑资源同步





资源更新：主要是常量缓冲的更新，目前考虑暴露接口逐帧修改uniform（数目较少时，开销应该相当于修改根常量）

前向渲染pass+一个后处理pass



**10.7**

* 定义了render pass，注入了FBO，VAO、materai manager
* 定义两个render pass的draw函数
* 定义renderpipeline，初始化renderpass并绘制
* 将renderpipeline的初始化和绘制集成到现有代码中



学到的东西：

unique管理声明周期，使用者获取裸指针并检查是否为空

FBO的管理逻辑：由底层(pipeline)创建和管理，高层(renderpass)只通过接口注入和持有引用；FBO需要在窗口变化时重建

我需要明确各层的资源管理职责

注意检查是否返回空引用

gltexture*创建不可变纹理，需要更新时重新创建



FBO RESIZE

完善绘制逻辑

添加纹理类型，framebuffer先创建引擎封装的纹理Texture，然后绑定到渲染目标

实例化绘制多个物体：UBO

添加后处理shader



post pass：
shader binding

VAO binding

bind input texture

reset some rendering states like depth test

2 pass:

render to offscreen texture

render to target fbo



build temporal texture ,and build fbo



10.9

完成前向渲染pass

将renderpipeline集成到引擎中



1. **智能指针使用**

类选用什么指针类型要考虑资源管理职责，使用shared_ptr，则一个类不能决定所引用对象的声明周期。

应该考虑使用unique_ptr+裸指针，或者shared_ptr+weak_ptr，管理所引用对象生命周期

使用前者可能会导致垂悬指针的问题，持有裸指针的对象是一种“观察者”，而持有unique_ptr的对象是资源的“所有者”，所有者不应比观察者先释放。



2. 多个类互相引用彼此的成员变量，如何保证**正确地初始化**？

需要分三步：

* 各个类分别初始化不依赖于其它类的成员
* 依赖注入：相互注入所有依赖
* 做一些与外部依赖于其他类成员的初始化工作



3. 通过**前向声明**，避免在头文件中包含有相关声明的头文件，可以尽可能减少依赖





editor和render之间数据交互？例如editor需要访问FBO最好的方式是？使用shared_ptr的坏处？使用对象而不是用指针？

使用ref<object>的好处？

* 类成员unique_ptr，类的默认析构函数将会使用到unique_ptr所指向对象的析构定义，因为在.h文件中定义了~renderlayer()，则析构函数会自动内联到.h文件中。



依赖注入，一些依赖并不需要在初始化时就注入



操作失败时报错，操作成功时，为了避免控制台太满，可以不用打印输出



create_shared_ptr是在实例化一个类型，抽象接口无法实例化

引擎抽象出平台无关的类型，作为抽象接口，要实例化平台相关的具体类型，使用它们的工厂方法，而不是实例化抽象接口
