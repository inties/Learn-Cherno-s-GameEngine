#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include "Engine/core.h"

namespace Engine {

    enum class AssetType { File, Directory, Unknown };

    struct AssetEntry {
        std::string relativePath; // ��� projectRoot
        AssetType type = AssetType::Unknown;
    };

    struct DirEntry {
        std::string name;         // �ļ�/�ļ�������
        std::string relativePath; // �����Ŀ����·��
        bool isDirectory = false;
        AssetType type = AssetType::Unknown;
    };

    enum class CopyStrategy {
        Skip,      // �����Ѵ��ڵ��ļ�
        Overwrite, // �����Ѵ��ڵ��ļ�
        Rename     // ���������ļ� (����: file.txt -> file (1).txt)
    };

    struct ImportResult {
        bool success = false;
        std::string message;
        std::string targetPath; // ���������·��
    };

    // ��Сʵ�ֵ���Ŀ��������ά����Ŀ��·������¼������Ŀ���ʲ���Ŀ
    class ProjectManager {
    public:
        ProjectManager();
        static Ref<ProjectManager> Get();

        void SetProjectRoot(const std::string& path);
        const std::string& GetProjectRoot() const { return m_ProjectRoot; }

        // �������·��������淶��Ϊ�����Ŀ��·������¼
        bool AddFileByBrowse(const std::string& absPath);

        // �����ⲿ�ļ�����Ŀ��Ŀ¼
        ImportResult CopyFileToProject(const std::string& sourceAbsPath, const std::string& targetRelDir = "", CopyStrategy strategy = CopyStrategy::Rename);
        
        // �����ⲿĿ¼����Ŀ��Ŀ¼
        std::vector<ImportResult> CopyDirectoryToProject(const std::string& sourceDirAbsPath, const std::string& targetRelDir = "", CopyStrategy strategy = CopyStrategy::Rename);
        
        // �����û�������ļ�·������֤����ӵ���Ŀ��
        ImportResult ProcessInputPath(const std::string& inputPath);

        // �о�ָ�����Ŀ¼�µ������ļ����ļ���
        std::vector<DirEntry> ListDirectoryContents(const std::string& relativeDir = "") const;

        // ˢ���ʲ��б�����ɨ����Ŀ��Ŀ¼��
        void RefreshAssets();

        const std::vector<AssetEntry>& ListAssets() const { return m_Assets; }

        // ʵ�ã�������չ���ж����ͣ���Сʵ�֣�
        static AssetType ClassifyByExtension(const std::string& extLower);

        // ������·���淶Ϊͳһ�ָ�����񣨽� '\' תΪ '/'������ȥ������ָ�
        static std::string NormalizePath(const std::string& p);

        // ���� absPath ��� m_ProjectRoot �����·����ʧ��ʱ���ؿ��ַ���
        std::string MakeRelativeToRoot(const std::string& absPath) const;

    private:
        std::string m_ProjectRoot; // ����·��
        std::vector<AssetEntry> m_Assets;
        std::unordered_set<std::string> m_AssetDedup; // ���أ����� relativePath��

        // ��������
        std::string GenerateUniqueFileName(const std::string& targetAbsPath) const;
        bool IsValidTargetPath(const std::string& targetRelPath) const;
        void ScanDirectoryRecursive(const std::string& dirAbsPath, const std::string& relativeBase);
    };
}