#pragma once
#include "pch.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Scene/Scene.h"

namespace Engine {
	struct RenderPipeLineSetting {
		ResourceRegistry<Material>* MatManager;
		ResourceRegistry<VertexArray>* VAOManager;
		ResourceRegistry<Texture>* TexManager;
		ResourceRegistry<Shader>* ShaderManager;
		Framebuffer* ScreenFBO;
		Scene* Scene;
		ShaderStorageBuffer* lights_gpu;
	};
}
