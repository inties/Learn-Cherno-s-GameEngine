#pragma once

// Standard library includes first
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
#include <chrono>
#include <cstdint>
// Engine core - before platform specific includes
#include "Engine/core.h"

// Platform specific includes
#ifdef ENGINE_PLATFORM_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

//---------------------第三方依赖库------------------------//
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "E:\myGitRepos\GameEngine\GameEngine\dependency\entt\src\entt\entt.hpp"
#include <json.hpp>//nolhmann
#include <imgui.h>
#include "Engine/Imgui/ImguiOpenglRender.h"
#include "Engine/Imgui/imgui_impl_glfw.h"


#include "Engine/log.h"
#include "Engine/Debug/Instrumentor.h"
