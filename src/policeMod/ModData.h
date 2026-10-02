#pragma once

#include <string>

class ModData {
public:
    std::string modFolderPath;

    ModData(std::string folderName);

    std::string GetFileFromAssets(const std::string& localPath);
    std::string GetFile(const std::string& localPath);
    std::string GetFileFromMenuSZK(const std::string& localPath);
    
    void LoadSettings();

    static void CreateFolder(const std::string& path);
};

extern ModData* modData;