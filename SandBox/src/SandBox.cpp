#include <Engine.h>
#include <Engine/EntryPoint.h>

class SandBox : public Engine::Application
{
public:
	SandBox() {

	}

	~SandBox() {
	}
};

Engine::Application* Engine::createApplication() {
	return new SandBox();
}
