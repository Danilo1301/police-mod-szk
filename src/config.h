#pragma once

#include "mod/iaml.h"
#include "src/config.h"

#include <string>
#include <sys/stat.h>

inline void CreateDirectoryIfNeeded(const std::string &path) { mkdir(path.c_str(), 0777); }

inline std::string GetModFolder(std::string modFolderName)
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();

    std::string folderName = modFolderName;

    std::string modFolder = dataRootPath + "/mods/data/" + folderName;

    CreateDirectoryIfNeeded(dataRootPath + "/mods");

    CreateDirectoryIfNeeded(dataRootPath + "/mods/data");

    CreateDirectoryIfNeeded(modFolder);

    CreateDirectoryIfNeeded(modFolder + "/assets");

    return modFolder + "/";
}

inline std::string GetModFolder() { return GetModFolder("policeModSZK"); }

inline std::string GetModAssetPath(const std::string &relativePath)
{
    return GetModFolder() + "/assets/" + relativePath;
}

inline std::string GetModFolderPath(const std::string &relativePath) { return GetModFolder() + relativePath; }

inline std::string GetMenuAssetPath(const std::string &relativePath)
{
    return GetModFolder("menuSZK") + "/assets/" + relativePath;
}