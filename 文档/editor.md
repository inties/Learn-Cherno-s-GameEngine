Editor�Ĺ����ص㣺
- ���������壨Panel������˾��ְ���ṩֱ�۵Ľ������档

- �ӿڴ��ڣ�Viewport����ʵʱչʾ������Ⱦ�������������ʾ����� Gizmos�������ֱ�����֧��������������彻����

- �����༭���ܣ�֧�ִ�����ɾ�����ƶ������š���ת���壬�Լ�ʰȡ��Picking���Ͷ�ѡ������

- ��Դ�����ܣ����롢�����͹���ģ�͡��������Դ��֧����ק�����·�������

- Inspector ���ڣ�չʾ���޸���ѡ��������ԣ����� Transform��λ��/��ת/���ţ������ʲ�������ɫ/������/�ֲڶ�/�����������չ��δ�� ECS ֧�֣���

- ��Դ�����ڣ������Ŀ�ʲ���Ԥ��ģ��/�����������ˣ������ɵ���/�������ߡ�

- ������壺Hierarchy���㼶��ͼ�����ڹ���������������Console����־����Ⱦͳ�ƣ��� FPS/DrawCalls����

- ģʽ�л���Editor/Runtime �л���֧�� Play/Pause/Step��Runtime ʱ��ͣ�༭��������Ϸ�߼���


�༭������������������ṹ���룩

һ��Ŀ���뷶Χ
- �Ե�ǰ����� Layer + ImGui �ܹ�Ϊ�������ṩ���õ� Editor ģʽ���㼶��Hierarchy�������ӣ�Inspector/Properties�����ӿڣ�Viewport��������̨��Console/RenderInfo������壬�Լ�������ѡȡ�������ƽ��/��ת/���� Gizmos����
- ���ڲ����� ECS ������ Runtime��Ĭ�Ͻ��� Editor ״̬����ͣ��Ϸѭ�������������� Runtime/Play ģʽ��

���������мܹ������ϵ�
- ��ѭ��ʱ��ÿ֡����ִ�и� Layer �� OnUpdate��Ȼ���� ImGui ��ܰ��� UI ���ƣ�ImGuiLayer::Begin() -> �� Layer::OnImGuiRender() -> ImGuiLayer::End()����ģʽ���� ImGui ��ʵ�֣�Begin �ڲ����� DockSpace �������֣�OnImGuiRender ��¶��������ڡ�
  �ο���<mcfile name="ImGuiLayer.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.cpp"></mcfile> <mcfile name="ImGuiLayer.h" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.h"></mcfile>
- �¼�ϵͳ��ƽ̨�ˣ�GLFW���ص�ע�뵽�����¼����У���ѭ��ͳһ�ַ����� Layer �� OnEvent��ImGuiLayer �ڲ���ʹ�� EventDispatcher �ַ����/����/�����¼����ҿ�ͨ�� ImGuiIO �� WantCaptureMouse/WantCaptureKeyboard �����¼����Ե�/͸������
  �ο���<mcfile name="ImGuiLayer.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.cpp"></mcfile> <mcfile name="�¼�ϵͳ.md" path="e:\myGitRepos\GameEngine\�ĵ�\�¼�ϵͳ.md"></mcfile>
- �����ѯ���ڱ༭��������ݼ��пɸ��� WindowsInput ���󣨻��� glfwGetKey / glfwGetMouseButton / glfwGetCursorPos����
  �ο���<mcfile name="WindowsInput.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\platform\WindowsInput.cpp"></mcfile>

����Layer ��֯������
- ���� EditorLayer����ͨ Layer������ Overlay����ְ��
  1) ��Ⱦ�༭������壨�� OnImGuiRender �ڣ���
  2) ������༭����ص��¼���״̬���� OnEvent/OnUpdate �ڣ���
  3) ����༭̬���ݣ���ǰ���������б���ǰѡ�ж���Gizmo ״̬��������Ƶȡ�
- LayerStack ����˳��ʾ������
  1) RendererLayer�����𳡾���Ⱦ/����֡���壩
  2) EditorLayer������ UI ��塢ת��/�����¼��������������ݣ�
  3) ImGuiLayer��Overlay��ֻ���� Begin/End �� DockSpace/���ӿڹ���
- ��������RendererLayer רע�������Ⱦ����EditorLayer ������Ⱦʲô + ��α��û���������EditorLayer �� OnImGuiRender �д� RendererLayer ��ȡ����ʱͳ��/��ȾĿ������Ƚ���չʾ����Ҫ�޸ĳ���ʱ��ͨ������� Scene/����ӿ�д�ء�

�ġ�Docking ����岼��
- DockSpace Host�������� ImGuiLayer::Begin �д�����ά�������轫�������д�� ImGuiLayer �ڣ���
- ��ʼ���ֽ��飺
  - ��ࣺHierarchy����״�ṹ���г������еĶ���
  - �ҲࣺInspector/Properties����ʾ���ɱ༭ Transform�����ʵȣ�
  - �ײ���Console/RenderInfo����־��֡��/DrawCall/�ڴ��ͳ�ƣ�
  - ���룺Viewport����ȾĿ����ʾ����֧������/���䣩
- ���������淶��"Hierarchy"��"Inspector"��"Viewport"��"Console"��"RenderInfo"���� Begin/End ��ǩ�ϸ�һ�£����� DockBuilder ���䲼�֣���

�塢Viewport ����Ⱦ·��
- ������Ⱦ��RendererLayer ��������Ⱦ��֡���壨Color+Depth������֧�� MSAA����EditorLayer �� Viewport ������� ImGui::Image չʾ Color ����
- �ӿڳߴ�仯���� Viewport ����С�仯ʱ���ؽ�/����֡����ߴ磻ά������Ⱦ�ֱ��ʡ��͡������ʾ�ֱ��ʡ���ӳ���� Letterbox �߼���ȷ�������졣
- ���껻�㣺�� ImGui ������������ת��Ϊ֡�����������꣬����ʰȡ/��ק��

��������ѡȡ��Picking���� Gizmos
- ѡȡ���������ȣ������� ID ����
  1) RendererLayer �������һ�� R32/UI ��ʽ�ġ�ObjectId����ɫ������
  2) �����ʱ�ڸø����������� ID ӳ�䵽����
  3) ֧�ֶ�ʵ��ʱ��ObjectId ���� DrawCall �������֣�ʵ�����ɱ��� InstanceId����
- ��ѡ����Ļ�ռ�����ʰȡ��Ray Cast��
  1) �����ͶӰ����Ļ���귴ͶӰ���������ߣ�
  2) ���Χ��/�������ཻ���ԣ�ȡ������С�
- Gizmos��ƽ��/��ת/���ţ�
  - ���ڣ����ü򻯰� Gizmo��ʹ�� ImDrawList ������/Բ��/�����ֱ������в�������Ļ�ռ���У���
  - ������������ ImGuizmo �Լ��ٿ�������ǰ�ֿ�δ���ɣ��ӿ�������չ�㣩��
  - ģʽ������ϵ��W/E/R �л� Translate/Rotate/Scale������/��������ϵ�л���Snap �������� 1/5/10 ��λ �� 5/15 �ȣ���
  - ����д�أ�Gizmo ������ı任��д������ Transform������ Inspector ��弴ʱͬ����ʾ��

�ߡ��༭�����������·��
- ������ƣ�ʾ��Ĭ�ϣ���
  - �Ҽ���ק + WASD�����ɷ��У�FPS��
  - �м���ק��ƽ�ƣ�Pan��
  - ���֣�����/ǰ����
  - F���۽���ѡ�ж���Frame Selected��
- ����·�ɹ���
  - �� ImGui::GetIO().WantCaptureMouse Ϊ�棬����¼������� 3D �ӿ��������
  - ������� Viewport ��������δ�� UI �������¼����� EditorLayer ���ӿڽ���/������ƣ�
  - ����ͬ��ο� WantCaptureKeyboard��ȫ�ֿ�ݼ����� Ctrl+S������δ�۽��ı�����ʱ��Ч��
- ���������װ��ͨ�� <mcfile name="WindowsInput.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\platform\WindowsInput.cpp"></mcfile> ��ѯ����״̬���� EditorLayer::OnEvent �� OnUpdate ���ۺ�ʹ�ã��¼����� + ��ѯ����

�ˡ�Inspector��Properties������ʱ༭
- Transform��λ��/��ת/���ţ�֧�ֶ���/�����л������ð�ť����һ��/�Ϸ����Ƕȣ���
- ���ʣ�������������ɫ�������ȡ��ֲڶȡ�������ͼѡ��/Ԥ���ȣ��������֪ͨ RendererLayer ˢ�°󶨡�
- �����չ�㣺��ǰ�� ECS�������ԡ���ѡģ�顱�ķ�ʽ���� Inspector �У��磺���������ƹ�������ű�ռλ����

�š�Hierarchy �볡������
- ���ݽṹ����ʱ Scene/Node �ṹά�����ӹ�ϵ��ɼ��ԣ�������ƽ��Ǩ�Ƶ� ECS��
- ������
  - ����/ɾ��/������/��ק���ţ��ı丸�ӹ�ϵ����
  - ���ļ�·������ģ�ͣ����ɵ�������Դ�������̣�����󽫸��ڵ���볡������
  - ѡ��ͬ��������ѡ�С�Ctrl/Shift ��ѡ��δ�������� Inspector/Viewport ͬ����

ʮ��Runtime/Editor ģʽ�л�
- Toolbar ��ť��Play / Pause / Step / Stop��
- �л����ԣ�
  - ���� Play������ Editor �����������ֻ����ǣ������� Gizmo�����ýű�/������£�
  - �˳� Play���ص��༭ǰ�ĳ���״̬�������ڽ��� Play ǰ��һ������ʱ���գ�����༭���ݱ�����ʱ�޸���Ⱦ����

ʮһ�����ͨ���������߽�
- RendererLayer ������ ImGui/Editor��EditorLayer ͨ���ӿڡ���ȡ��ͳ������/֡������������ ID�����Լ������͡��������������/ɾ�����󡢲��ʲ����޸ģ���
- ImGuiLayer �����ؾ���ҵ����壬���ṩ Begin/End ����� DockSpace��

ʮ������ݼ�Լ�������飩
- W/E/R��ƽ��/��ת/����
- Q���� Gizmo��ѡ��ģʽ��
- F���۽�ѡ�ж���
- Delete��ɾ��ѡ�ж���
- Ctrl+D������ѡ�ж���
- Ctrl+S�����泡����ռλ��

ʮ��������/������״̬�־û����׶��� TODO��
- ���������ʽ��JSON/�Զ�������ƣ��׶� 1 ���� JSON�����ڵ��ԣ���
- ���ݣ�����㼶��Transform������/�������á����/�ƹ⡢�������á�
- ��Դ·����ͳһ��Թ��̸�·�����������·��������

ʮ�ġ�ʵ�ֲ��裨��̱���
- M1��EditorLayer �
  1) �½� EditorLayer ������ LayerStack��
  2) �� OnImGuiRender ��ʵ�� DockSpace �µ��ĸ�������壨Hierarchy/Inspector/Viewport/Console����
  3) RendererLayer �������֡���壬Viewport ��ʾ�������ߴ�����Ӧ��
- M2��������ѡȡ
  1) Viewport �������任��߽��жϣ�
  2) ����ʰȡ�� ID ����ѡȡ������ ID ���壩��
  3) ѡ�и����� Inspector ͬ����
  4) �༭���������������·�����ơ�
- M3��Gizmos ��任д��
  1) �򻯰�����/��ת��/�����ֱ���
  2) ֧������/���ء�Snap��
  3) �� Transform/��Ⱦͬ����
- M4���ʲ�������㼶����
  1) ���ļ�·������ģ�Ͳ����볡����
  2) Hierarchy ��ק���š�ɾ��/���ƣ�
  3) Console/RenderInfo �������ʱͳ�ƣ����� RendererLayer����
- M5��Runtime/Editor �л�����
  1) Toolbar ��״̬����
  2) ����/�˳� Play �ĳ������գ�
  3) �����ű����¿��ء�

ʮ�塢ע������
- ���ӿڣ�ViewportsEnable���� Docking ���� ImGui ���ʼ��������һ�£�
- �¼������¡���ѭ ImGuiIO ��־������ UI �� 3D �����໥���ţ�
- ����ϵ�뵥λͳһ������/���֡���/�������ŵ�λ����
- ֡�����ؽ�����Դ�������ڹ�������Ƶ���������ٵ��¶�������
- ���ܻ�׼����ԣ��� Console/RenderInfo ��¶ FPS��CPU/GPU ʱ�䡢DrawCalls������������ָ�ꡣ

�ο��ļ�
- ImGui ����벼�֣�<mcfile name="ImGuiLayer.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.cpp"></mcfile> <mcfile name="ImGuiLayer.h" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.h"></mcfile>
- �¼�ϵͳ�����룺<mcfile name="�¼�ϵͳ.md" path="e:\myGitRepos\GameEngine\�ĵ�\�¼�ϵͳ.md"></mcfile> <mcfile name="WindowsInput.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\platform\WindowsInput.cpp"></mcfile>