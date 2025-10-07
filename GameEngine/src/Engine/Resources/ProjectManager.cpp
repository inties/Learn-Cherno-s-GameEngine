#include "pch.h"
#include "Engine/Resources/ProjectManager.h"
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

namespace Engine {

    static std::string ToLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)std::tolower(c); });
        return s;
    }

    ProjectManager::ProjectManager() {
        // ??????????????
        SetProjectRoot("E:/myGitRepos/GameEngine/SandBox");
        std::cout << "[ProjectManager] Default project root set to: E:/myGitRepos/GameEngine/SandBox" << std::endl;
    }

    Ref<ProjectManager> ProjectManager::Get() {
        static Ref<ProjectManager> s_Instance = CreateRef<ProjectManager>();
        return s_Instance;
    }

    void ProjectManager::SetProjectRoot(const std::string& path) {
        if (path.empty()) { m_ProjectRoot.clear(); return; }
        fs::path p(path);
        m_ProjectRoot = p.lexically_normal().string();
#ifdef _WIN32
        // ?????潩?????????潩????????
        std::replace(m_ProjectRoot.begin(), m_ProjectRoot.end(), '\\', '/');
#endif
    }

    AssetType ProjectManager::ClassifyByExtension(const std::string& extLower) {
        // ?????????????????????????????????????????
        if (!extLower.empty()) {
            return AssetType::File;
        }
        return AssetType::Unknown;
    }

    std::string ProjectManager::NormalizePath(const std::string& p) {
        if (p.empty()) return {};
        fs::path fp(p);
        std::string s = fp.lexically_normal().string();
#ifdef _WIN32
        std::replace(s.begin(), s.end(), '\\', '/');
#endif
        return s;
    }

    std::string ProjectManager::MakeRelativeToRoot(const std::string& absPath) const {
        if (m_ProjectRoot.empty()) return {};
        std::error_code ec;
        fs::path root(m_ProjectRoot);
        fs::path ap(absPath);
        fs::path rel = fs::relative(ap, root, ec);
        if (ec) return {};
        std::string r = rel.string();
#ifdef _WIN32
        std::replace(r.begin(), r.end(), '\\', '/');
#endif
        return r;
    }

    bool ProjectManager::AddFileByBrowse(const std::string& absPath) {
        if (m_ProjectRoot.empty()) return false;
        fs::path p(absPath);
        if (!fs::exists(p)) return false;
        std::string rel = MakeRelativeToRoot(absPath);
        if (rel.empty()) return false;
        rel = NormalizePath(rel);
        if (m_AssetDedup.count(rel)) return true; // ????????????
        std::string ext = ToLower(fs::path(rel).extension().string());
        AssetType type = ClassifyByExtension(ext);
        m_Assets.push_back({ rel, type });
        m_AssetDedup.insert(rel);
        return true;
    }

    ImportResult ProjectManager::CopyFileToProject(const std::string& sourceAbsPath, const std::string& targetRelDir, CopyStrategy strategy) {
        ImportResult result;
        
        std::cout << "[ProjectManager] CopyFileToProject: " << sourceAbsPath << " -> " << targetRelDir << std::endl;
        
        if (m_ProjectRoot.empty()) {
            result.message = "Project root not set";
            std::cout << "[ProjectManager] Error: Project root not set" << std::endl;
            return result;
        }
        
        fs::path sourcePath(sourceAbsPath);
        if (!fs::exists(sourcePath) || !fs::is_regular_file(sourcePath)) {
            result.message = "Source file does not exist or is not a regular file";
            std::cout << "[ProjectManager] Error: Source file invalid - " << sourceAbsPath << std::endl;
            return result;
        }
        
        // ???????潩??
        std::string fileName = sourcePath.filename().string();
        std::string targetRelPath = targetRelDir.empty() ? fileName : (NormalizePath(targetRelDir) + "/" + fileName);
        
        if (!IsValidTargetPath(targetRelPath)) {
            result.message = "Invalid target path (outside project root)";
            return result;
        }
        
        fs::path targetAbsPath = fs::path(m_ProjectRoot) / targetRelPath;
        
        // ????????????
        fs::path targetDir = targetAbsPath.parent_path();
        std::error_code ec;
        fs::create_directories(targetDir, ec);
        if (ec) {
            result.message = "Failed to create target directory: " + ec.message();
            return result;
        }
        
        // ??????????
        if (fs::exists(targetAbsPath)) {
            switch (strategy) {
                case CopyStrategy::Skip:
                    result.success = true;
                    result.message = "File already exists, skipped";
                    result.targetPath = NormalizePath(targetRelPath);
                    return result;
                    
                case CopyStrategy::Overwrite:
                    // ??????潩???
                    break;
                    
                case CopyStrategy::Rename:
                    targetAbsPath = GenerateUniqueFileName(targetAbsPath.string());
                    targetRelPath = MakeRelativeToRoot(targetAbsPath.string());
                    break;
            }
        }
        
        // ??????????
        std::cout << "[ProjectManager] Copying file to: " << targetAbsPath.string() << std::endl;
        fs::copy_file(sourcePath, targetAbsPath, fs::copy_options::overwrite_existing, ec);
        if (ec) {
            result.message = "Failed to copy file: " + ec.message();
            std::cout << "[ProjectManager] Error: Failed to copy file - " << ec.message() << std::endl;
            return result;
        }
        
        // ????????潩?
        std::string normalizedRelPath = NormalizePath(targetRelPath);
        if (!m_AssetDedup.count(normalizedRelPath)) {
            std::string ext = ToLower(fs::path(normalizedRelPath).extension().string());
            AssetType type = ClassifyByExtension(ext);
            m_Assets.push_back({ normalizedRelPath, type });
            m_AssetDedup.insert(normalizedRelPath);
            std::cout << "[ProjectManager] Asset added to list: " << normalizedRelPath << std::endl;
        }
        
        result.success = true;
        result.message = "File copied successfully";
        result.targetPath = normalizedRelPath;
        std::cout << "[ProjectManager] File copy completed successfully: " << normalizedRelPath << std::endl;
        return result;
    }

    std::vector<ImportResult> ProjectManager::CopyDirectoryToProject(const std::string& sourceDirAbsPath, const std::string& targetRelDir, CopyStrategy strategy) {
        std::vector<ImportResult> results;
        
        std::cout << "[ProjectManager] CopyDirectoryToProject: " << sourceDirAbsPath << " -> " << targetRelDir << std::endl;
        
        if (m_ProjectRoot.empty()) {
            ImportResult result;
            result.message = "Project root not set";
            std::cout << "[ProjectManager] Error: Project root not set for directory copy" << std::endl;
            results.push_back(result);
            return results;
        }
        
        fs::path sourceDir(sourceDirAbsPath);
        if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
            ImportResult result;
            result.message = "Source directory does not exist or is not a directory";
            std::cout << "[ProjectManager] Error: Source directory invalid - " << sourceDirAbsPath << std::endl;
            results.push_back(result);
            return results;
        }
        
        std::cout << "[ProjectManager] Starting recursive directory copy..." << std::endl;
        
        // ??漙?????潩????????
        std::error_code ec;
        int fileCount = 0;
        for (const auto& entry : fs::recursive_directory_iterator(sourceDir, ec)) {
            if (ec) {
                ImportResult result;
                result.message = "Error iterating directory: " + ec.message();
                results.push_back(result);
                continue;
            }
            
            if (entry.is_regular_file()) {
                fileCount++;
                std::cout << "[ProjectManager] Processing file " << fileCount << ": " << entry.path().filename().string() << std::endl;
                
                // ??????????????潩??
                fs::path relativePath = fs::relative(entry.path(), sourceDir, ec);
                if (ec) continue;
                
                // ??????????潩??
                std::string targetSubDir = targetRelDir.empty() ? 
                    relativePath.parent_path().string() : 
                    (NormalizePath(targetRelDir) + "/" + relativePath.parent_path().string());
                
                ImportResult result = CopyFileToProject(entry.path().string(), targetSubDir, strategy);
                results.push_back(result);
            }
        }
        
        std::cout << "[ProjectManager] Directory copy completed. Total files processed: " << fileCount << std::endl;
        
        return results;
    }

    std::vector<DirEntry> ProjectManager::ListDirectoryContents(const std::string& relativeDir) const {
        std::vector<DirEntry> entries;
        
        if (m_ProjectRoot.empty()) {
            return entries;
        }
        
        fs::path targetDir = fs::path(m_ProjectRoot);
        if (!relativeDir.empty()) {
            targetDir /= NormalizePath(relativeDir);
        }
        
        if (!fs::exists(targetDir) || !fs::is_directory(targetDir)) {
            return entries;
        }
        
        std::error_code ec;
        for (const auto& entry : fs::directory_iterator(targetDir, ec)) {
            if (ec) continue;
            
            DirEntry dirEntry;
            dirEntry.name = entry.path().filename().string();
            dirEntry.isDirectory = entry.is_directory();
            
            // ???????潩??
            std::string entryRelPath = MakeRelativeToRoot(entry.path().string());
            dirEntry.relativePath = NormalizePath(entryRelPath);
            
            // ?潩?????????????
            if (dirEntry.isDirectory) {
                dirEntry.type = AssetType::Directory;
            } else {
                std::string ext = ToLower(entry.path().extension().string());
                dirEntry.type = ClassifyByExtension(ext);
            }
            
            entries.push_back(dirEntry);
        }
        
        // ?????????????????????????
        std::sort(entries.begin(), entries.end(), [](const DirEntry& a, const DirEntry& b) {
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory > b.isDirectory; // ?????
            }
            return a.name < b.name;
        });
        
        return entries;
    }

    void ProjectManager::RefreshAssets() {
        m_Assets.clear();
        m_AssetDedup.clear();
        
        if (m_ProjectRoot.empty() || !fs::exists(m_ProjectRoot)) {
            return;
        }
        
        ScanDirectoryRecursive(m_ProjectRoot, "");
    }

    std::string ProjectManager::GenerateUniqueFileName(const std::string& targetAbsPath) const {
        fs::path originalPath(targetAbsPath);
        fs::path dir = originalPath.parent_path();
        std::string stem = originalPath.stem().string();
        std::string ext = originalPath.extension().string();
        
        int counter = 1;
        fs::path newPath;
        do {
            std::string newName = stem + " (" + std::to_string(counter) + ")" + ext;
            newPath = dir / newName;
            counter++;
        } while (fs::exists(newPath));
        
        return newPath.string();
    }

    bool ProjectManager::IsValidTargetPath(const std::string& targetRelPath) const {
        if (targetRelPath.empty()) return false;
        
        // ?????????潩???潩?????
        fs::path path(targetRelPath);
        for (const auto& component : path) {
            std::string comp = component.string();
            if (comp == ".." || comp.find(':') != std::string::npos) {
                return false;
            }
        }
        
        // ????漐?????潩?????????????????
        fs::path fullPath = fs::path(m_ProjectRoot) / targetRelPath;
        std::error_code ec;
        fs::path canonical = fs::weakly_canonical(fullPath, ec);
        if (ec) return false;
        
        fs::path rootCanonical = fs::weakly_canonical(m_ProjectRoot, ec);
        if (ec) return false;
        
        std::string canonicalStr = canonical.string();
        std::string rootStr = rootCanonical.string();
        
        return canonicalStr.substr(0, rootStr.length()) == rootStr;
    }

    void ProjectManager::ScanDirectoryRecursive(const std::string& dirAbsPath, const std::string& relativeBase) {
        std::error_code ec;
        for (const auto& entry : fs::recursive_directory_iterator(dirAbsPath, ec)) {
            if (ec) continue;
            
            if (entry.is_regular_file()) {
                std::string relPath = MakeRelativeToRoot(entry.path().string());
                if (!relPath.empty() && !m_AssetDedup.count(relPath)) {
                    std::string ext = ToLower(entry.path().extension().string());
                    AssetType type = ClassifyByExtension(ext);
                    m_Assets.push_back({ relPath, type });
                    m_AssetDedup.insert(relPath);
                }
            }
        }
    }

    ImportResult ProjectManager::ProcessInputPath(const std::string& inputPath) {
        ImportResult result;
        
        if (inputPath.empty()) {
            result.message = "Please enter a valid path";
            return result;
        }
        
        std::filesystem::path path(inputPath);
        if (!std::filesystem::exists(path)) {
            result.message = "Path does not exist: " + inputPath;
            return result;
        }
        
        try {
            if (std::filesystem::is_directory(path)) {
                // ?????????
                auto results = CopyDirectoryToProject(inputPath);
                if (!results.empty() && results[0].success) {
                    result.success = true;
                    result.message = "Directory copied successfully: " + path.filename().string();
                    result.targetPath = results[0].targetPath;
                } else {
                    result.message = results.empty() ? "Failed to copy directory" : results[0].message;
                }
            } else {
                // ???????
                result = CopyFileToProject(inputPath);
                if (result.success) {
                    result.message = "File copied successfully: " + path.filename().string();
                }
            }
            
            if (result.success) {
                RefreshAssets(); // ???????潩?
            }
        } catch (const std::exception& e) {
            result.message = "Copy failed: " + std::string(e.what());
        }
        
        return result;
    }
}