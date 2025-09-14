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

// Engine core - before platform specific includes
#include "Engine/core.h"

// Platform specific includes
#ifdef ENGINE_PLATFORM_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

//---------------------第三方依赖库------------------------//
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/log.h"
#include "Engine/Debug/Instrumentor.h"
