#pragma once

#include <Engine.h>
#include<memory>
class SandBox : public Engine::Application
{
public:
	SandBox();
	virtual ~SandBox();
	
	void run() override;
	
private:
};
