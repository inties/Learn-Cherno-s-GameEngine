

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



FBO RESIZE

完善绘制逻辑

添加纹理类型，framebuffer先创建纹理，然后绑定到渲染目标

添加后处理shader

