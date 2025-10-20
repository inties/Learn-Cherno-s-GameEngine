#include "pch.h"
#include "Prefilter.h"
#include <stb_image.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

Prefilter::Prefilter() : m_Width(0), m_Height(0), m_Channels(0) {
}

Prefilter::~Prefilter() {
}

bool Prefilter::LoadHDRTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    
    float* data = stbi_loadf(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    if (!data) {
        std::cerr << "Failed to load HDR image: " << path << std::endl;
        return false;
    }
    
    // 复制数据到成员变量
    size_t dataSize = m_Width * m_Height * m_Channels;
    m_HDRData.resize(dataSize);
    std::memcpy(m_HDRData.data(), data, dataSize * sizeof(float));
    
    stbi_image_free(data);
    
    std::cout << "Loaded HDR texture: " << path 
              << " (" << m_Width << "x" << m_Height << ", " << m_Channels << " channels)" << std::endl;
    return true;
}

glm::vec2 Prefilter::GetUV(glm::vec3 dir) {
    // 将方向向量转换为球面坐标，再转换为UV坐标
    // 假设HDR贴图是等距柱状投影（equirectangular projection）
    dir = glm::normalize(dir);
    float safeY = glm::clamp(dir.y, -1.0f, 1.0f);
    float u = 0.5f + std::atan2(dir.z, dir.x) / (2.0f * PI);
    float v = 0.5f - std::asin(safeY) / PI;
    // 包裹到 [0,1)
    u = u - std::floor(u);
    v = v - std::floor(v);
    return glm::vec2(u, v);
}

glm::vec3 Prefilter::GetDirection(glm::vec2 uv) {
    // 将UV坐标转换为方向向量（等距柱状投影的逆变换）
    float u = uv.x - std::floor(uv.x);
    float v = uv.y - std::floor(uv.y);
    float phi = (u - 0.5f) * 2.0f * PI;
    float theta = (0.5f - v) * PI;
    
    float x = std::cos(theta) * std::sin(phi);
    float y = std::sin(theta);
    float z = std::cos(theta) * std::cos(phi);
    
    return glm::normalize(glm::vec3(x, y, z));
}

glm::vec3 Prefilter::Convolution(glm::vec2 uv, uint32_t sampleNum) {
    glm::vec3 normal = GetDirection(uv);
    glm::vec3 irradiance = glm::vec3(0.0f);
    
    // 使用 Frisvad 稳健正交基构建，避免 normal 接近 up 向量时退化
    glm::vec3 right, up;
    if (normal.z < -0.9999999f) {
        right = glm::vec3(0.0f, -1.0f, 0.0f);
        up    = glm::vec3(-1.0f, 0.0f, 0.0f);
    } else {
        float a = 1.0f / (1.0f + normal.z);
        float b = -normal.x * normal.y * a;
        right = glm::normalize(glm::vec3(1.0f - normal.x * normal.x * a, b, -normal.x));
        up    = glm::normalize(glm::vec3(b, 1.0f - normal.y * normal.y * a, -normal.y));
    }
    
    float sampleDelta = PI / std::max(1.0f, std::sqrt(static_cast<float>(sampleNum)));
    float nrSamples = 0.0f;
    
    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta) {
        for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta) {
            // 球面坐标转笛卡尔坐标（切线空间）
            glm::vec3 tangentSample = glm::vec3(
                std::sin(theta) * std::cos(phi),
                std::sin(theta) * std::sin(phi),
                std::cos(theta)
            );
            
            // 切线空间转世界空间
            glm::vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            
            // 从HDR贴图采样
            glm::vec2 sampleUV = GetUV(sampleVec);
            glm::vec3 sampleColor = SampleHDR(sampleUV);
            
            irradiance += sampleColor * std::cos(theta) * std::sin(theta);
            nrSamples++;
        }
    }
    
    irradiance = PI * irradiance * (1.0f / nrSamples);
    return irradiance;
}

glm::vec3 Prefilter::SampleHDR(glm::vec2 uv) {
    // 将UV坐标转换为像素坐标（U 环绕，V 夹取，避免极点缝隙）
    float u = uv.x - std::floor(uv.x);
    float v = glm::clamp(uv.y, 0.0f, 0.999999f);
    int x = static_cast<int>(u * m_Width) % m_Width;
    int y = static_cast<int>(v * m_Height);
    
    // 计算像素索引
    int pixelIndex = (y * m_Width + x) * m_Channels;
    
    if (pixelIndex + 2 >= m_HDRData.size()) {
        return glm::vec3(0.0f);
    }
    
    // 返回RGB值
    return glm::vec3(
        m_HDRData[pixelIndex],
        m_HDRData[pixelIndex + 1],
        m_HDRData[pixelIndex + 2]
    );
}

void Prefilter::GeneratePrefilterMap(const std::string& outputPath) {
    if (m_HDRData.empty()) {
        std::cerr << "No HDR data loaded!" << std::endl;
        return;
    }
    
    const int outputWidth = m_Width;
    const int outputHeight = m_Height;
    std::vector<float> irradianceData(outputWidth * outputHeight * 3);
    
    std::cout << "Generating irradiance map..." << std::endl;
    
    for (int y = 0; y < outputHeight; ++y) {
        for (int x = 0; x < outputWidth; ++x) {
            // 计算UV坐标
            float u = static_cast<float>(x) / outputWidth;
            float v = static_cast<float>(y) / outputHeight;
            
            // 计算辐照度
            glm::vec3 irradiance = Convolution(glm::vec2(u, v), 1024);
            
            // 存储结果
            int pixelIndex = (y * outputWidth + x) * 3;
            irradianceData[pixelIndex] = irradiance.r;
            irradianceData[pixelIndex + 1] = irradiance.g;
            irradianceData[pixelIndex + 2] = irradiance.b;
        }
        
        if (y % 32 == 0) {
            std::cout << "Progress: " << (y * 100 / outputHeight) << "%" << std::endl;
        }
    }
    
    // 保存HDR文件
    SaveHDRFile(outputPath, irradianceData, outputWidth, outputHeight);
    std::cout << "Irradiance map saved to: " << outputPath << std::endl;
}

void Prefilter::SaveHDRFile(const std::string& path, const std::vector<float>& data, int width, int height) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return;
    }
    
    // 写入HDR文件头
    file << "#?RADIANCE\n";
    file << "FORMAT=32-bit_rle_rgbe\n";
    file << "EXPOSURE=1.0\n";
    file << "SOFTWARE=GameEngine Prefilter\n";
    file << "\n";
    file << "-Y " << height << " +X " << width << "\n";
    
    // 写入RGBE数据（无RLE，直接逐像素）
    for (int i = 0; i < width * height; ++i) {
        float r = data[i * 3];
        float g = data[i * 3 + 1];
        float b = data[i * 3 + 2];
        
        // 转换为RGBE格式（Greg Ward 方法）
        float maxVal = std::max({ r, g, b });
        if (maxVal < 1e-32f) {
            file.put(0);
            file.put(0);
            file.put(0);
            file.put(0);
        } else {
            int e;
            float v = std::frexp(maxVal, &e); // maxVal = v * 2^e, v in [0.5,1)
            float scale = std::ldexp(256.0f, -e); // 256 / 2^e

            unsigned char rByte = static_cast<unsigned char>(glm::clamp(r * scale, 0.0f, 255.0f));
            unsigned char gByte = static_cast<unsigned char>(glm::clamp(g * scale, 0.0f, 255.0f));
            unsigned char bByte = static_cast<unsigned char>(glm::clamp(b * scale, 0.0f, 255.0f));

            file.put(rByte);
            file.put(gByte);
            file.put(bByte);
            file.put(static_cast<unsigned char>(e + 128));
        }
    }
    
    file.close();
}