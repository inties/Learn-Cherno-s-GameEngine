#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include "Engine/core.h"

namespace Engine {

    enum class AssetType { File, Directory, Unknown };

    struct AssetEntry {
        std::string relativePath; // 相对 projectRoot
        AssetType type = AssetType::Unknown;
    };

    struct DirEntry {
        std::string name;         // 文件/文件夹名称
        std::string relativePath; // 相对项目根的路径
        bool isDirectory = false;
        AssetType type = AssetType::Unknown;
    };

    enum class CopyStrategy {
        Skip,      // 跳过已存在的文件
        Overwrite, // 覆盖已存在的文件
        Rename     // 重命名新文件 (例如: file.txt -> file (1).txt)
    };

    struct ImportResult {
        bool success = false;
        std::string message;
        std::string targetPath; // 导入后的相对路径
    };

    // 最小实现的项目管理器：维护项目根路径，记录加入项目的资产条目
    class ProjectManager {
    public:
        ProjectManager();
        static Ref<ProjectManager> Get();

        void SetProjectRoot(const std::string& path);
        const std::string& GetProjectRoot() const { return m_ProjectRoot; }

        // 传入绝对路径，将其规范化为相对项目根路径并记录
        bool AddFileByBrowse(const std::string& absPath);

        // 复制外部文件到项目根目录
        ImportResult CopyFileToProject(const std::string& sourceAbsPath, const std::string& targetRelDir = "", CopyStrategy strategy = CopyStrategy::Rename);
        
        // 复制外部目录到项目根目录
        std::vector<ImportResult> CopyDirectoryToProject(const std::string& sourceDirAbsPath, const std::string& targetRelDir = "", CopyStrategy strategy = CopyStrategy::Rename);
        
        // 处理用户输入的文件路径（验证并添加到项目）
        ImportResult ProcessInputPath(const std::string& inputPath);

        // 列举指定相对目录下的所有文件和文件夹
        std::vector<DirEntry> ListDirectoryContents(const std::string& relativeDir = "") const;

        // 刷新资产列表（重新扫描项目根目录）
        void RefreshAssets();

        const std::vector<AssetEntry>& ListAssets() const { return m_Assets; }

        // 实用：基于扩展名判定类型（最小实现）
        static AssetType ClassifyByExtension(const std::string& extLower);

        // 将任意路径规范为统一分隔符风格（将 '\' 转为 '/'），并去除冗余分隔
        static std::string NormalizePath(const std::string& p);

        // 计算 absPath 相对 m_ProjectRoot 的相对路径；失败时返回空字符串
        std::string MakeRelativeToRoot(const std::string& absPath) const;

    private:
        std::string m_ProjectRoot; // 绝对路径
        std::vector<AssetEntry> m_Assets;
        std::unordered_set<std::string> m_AssetDedup; // 防重（基于 relativePath）

        // 辅助方法
        std::string GenerateUniqueFileName(const std::string& targetAbsPath) const;
        bool IsValidTargetPath(const std::string& targetRelPath) const;
        void ScanDirectoryRecursive(const std::string& dirAbsPath, const std::string& relativeBase);
    };
}