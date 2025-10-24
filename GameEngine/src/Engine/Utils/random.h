#pragma once
#include "pch.h"
namespace Engine {
	inline float random01(){
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);		
	}
	inline glm::vec3 random_vector3() {
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) ;
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) ;
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) ;
		return glm::vec3(x, y, z);
	}
}
