#include "pch.h"
#include "Texture.h"
#include <gtest/gtest.h>

namespace Engine {
    TEST(TextureCubeTest, CreateWithFaces) {
        std::vector<std::string> faces = {
            "right.jpg", "left.jpg", "top.jpg",
            "bottom.jpg", "front.jpg", "back.jpg"
        };
        auto texture = TextureCube::Create(faces);
        ASSERT_NE(texture, nullptr);
    }

    TEST(TextureCubeTest, CreateWithSingleFace) {
        auto texture = TextureCube::Create("single.jpg");
        ASSERT_NE(texture, nullptr);
    }

    TEST(TextureCubeTest, InvalidFaces) {
        std::vector<std::string> emptyFaces;
        auto texture = TextureCube::Create(emptyFaces);
        ASSERT_EQ(texture, nullptr);
    }

    TEST(TextureCubeTest, NonExistentFaces) {
        std::vector<std::string> nonExistentFaces = {
            "nonexistent1.jpg", "nonexistent2.jpg"
        };
        auto texture = TextureCube::Create(nonExistentFaces);
        ASSERT_EQ(texture, nullptr);
    }
}