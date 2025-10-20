#pragma once
#include "pch.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Prefilter {
public:
    Prefilter();
    ~Prefilter();
    
    bool LoadHDRTexture(const std::string& path); // 读取 HDR 文件
    void GeneratePrefilterMap(const std::string& outputPath); // 生成预过滤贴图

private:
    glm::vec2 GetUV(glm::vec3 dir); // 根据方向向量获取HDR贴图的 uv 坐标
    glm::vec3 GetDirection(glm::vec2 uv); // 根据 uv 坐标获取方向向量
    glm::vec3 Convolution(glm::vec2 uv, uint32_t sampleNum); // 黎曼积分计算卷积
    glm::vec3 SampleHDR(glm::vec2 uv); // 从HDR数据中采样
    void SaveHDRFile(const std::string& path, const std::vector<float>& data, int width, int height); // 保存HDR文件
    
    // HDR 数据
    std::vector<float> m_HDRData;
    int m_Width, m_Height, m_Channels;
    
    // 常量
    static constexpr float PI = 3.14159265359f;
};