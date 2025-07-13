#pragma once
#ifdef ENGINE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

// Engine core - 必须在其他包含之前
#include "Engine/core.h"

#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <utility>
#include <memory>



//---------------------其它依赖库------------------------//
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>