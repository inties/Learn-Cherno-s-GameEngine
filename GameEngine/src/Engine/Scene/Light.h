#pragma once
#include "pch.h"
namespace Engine {
	enum class LightType {
		direct,
		spot,
		point
	};
	struct Light {
	public:
		glm::vec3 strenth;
		float falloff_start;
		glm::vec3 direction;
		float falloff_end;
		glm::vec3 position;
		float spot_power;

		// 默认构造函数
		Light() : strenth(1.0f), falloff_start(1.0f), direction(0.0f, -1.0f, 0.0f), 
				  falloff_end(10.0f), position(0.0f), spot_power(1.0f) {}

		// 带参数的构造函数
		Light(const glm::vec3& strength, const glm::vec3& pos, const glm::vec3& dir = glm::vec3(0.0f, -1.0f, 0.0f),
			  float falloffStart = 1.0f, float falloffEnd = 10.0f, float spotPower = 3.0f)
			: strenth(strength), position(pos), direction(dir), falloff_start(falloffStart),
			  falloff_end(falloffEnd), spot_power(spotPower){}

	};
	struct PointLight : public Light {
		// 默认构造函数
		PointLight() : Light() {

		}

		// 带参数的构造函数
		PointLight(const glm::vec3& strength, const glm::vec3& pos, float falloffStart = 1.0f, float falloffEnd = 50.0f)
			: Light(strength, pos, glm::vec3(0.0f), falloffStart, falloffEnd, 1.0f) {

		}
	};

	struct DirectionalLight : public Light {
		// 默认构造函数
		DirectionalLight() : Light() {

			// 定向光源不需要位置，使用默认方向
			direction = glm::vec3(0.0f, -1.0f, 0.0f);
		}

		// 带参数的构造函数
		DirectionalLight(const glm::vec3& strength, const glm::vec3& dir)
			: Light(strength, glm::vec3(0.0f), dir, 0.0f, 0.0f, 1.0f) {

		}
	};

	struct SpotLight : public Light {
		// 默认构造函数
		SpotLight() : Light() {

			// 聚光灯默认向下照射
			direction = glm::vec3(0.0f, -1.0f, 0.0f);
			spot_power = 1.0f;
		}

		// 带参数的构造函数
		SpotLight(const glm::vec3& strength, const glm::vec3& pos, const glm::vec3& dir=glm::vec3(0.0f, -1.0f, 0.0f),
				  float falloffStart = 1.0f, float falloffEnd = 50.0f, float spotPower = 3.0f)
			: Light(strength, pos, dir, falloffStart, falloffEnd, spotPower) {

		}
	};

	// 主光源单例类
	class ENGINE_API MainLight {
	public:
		// 获取单例实例
		static MainLight& GetInstance() {
			static MainLight instance;
			return instance;
		}

		// 禁用拷贝构造和赋值
		MainLight(const MainLight&) = delete;
		MainLight& operator=(const MainLight&) = delete;

		// 获取当前主光源
		const DirectionalLight& GetLight() const { return m_Light; }

		// 设置光源方向
		void SetDirection(const glm::vec3& direction) {
			m_Light.direction = glm::normalize(direction);
		}

		// 设置光源强度
		void SetIntensity(float intensity) {
			m_Intensity = intensity;
			UpdateLightStrength();
		}

		// 设置光源颜色
		void SetColor(const glm::vec3& color) {
			m_Color = color;
			UpdateLightStrength();
		}

		// 获取光源方向
		const glm::vec3& GetDirection() const { return m_Light.direction; }

		// 获取光源强度
		float GetIntensity() const { return m_Intensity; }

		// 获取光源颜色
		const glm::vec3& GetColor() const { return m_Color; }

		// 获取光源强度向量（颜色 * 强度）
		const glm::vec3& GetStrength() const { return m_Light.strenth; }

	private:
		MainLight() : m_Light(glm::vec3(1.0f), glm::vec3(0.5f, -1.0f, 0.3f)), 
					  m_Intensity(1.0f), m_Color(1.0f, 1.0f, 1.0f) {
			UpdateLightStrength();
		}

		void UpdateLightStrength() {
			m_Light.strenth = m_Color * m_Intensity;
		}

		DirectionalLight m_Light;
		float m_Intensity;
		glm::vec3 m_Color;
	};
}

