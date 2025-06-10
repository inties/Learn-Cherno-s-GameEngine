#pragma once
extern Engine::Application* Engine::createApplication();
int main() {
	Engine::Application* app = Engine::createApplication();
	app->run();
	delete app;
	return 0;
}