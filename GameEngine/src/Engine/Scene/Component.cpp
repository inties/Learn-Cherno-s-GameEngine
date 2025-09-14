#include "pch.h"
#include "Component.h"
#include "ScriptableEntity.h"

namespace Engine {
	void DestroyScriptInstance(ScriptableEntity*& instance)
	{
		delete instance;
		instance = nullptr;
	}
}
