#include "ModData.h"

#include "../pch.h"

#include "../utils/utils.h"

#include <mod/amlmod.h>
#include <sys/stat.h>
#include "../utils/iniReaderWriter.hpp"

#include "Callouts.h"

ModData::ModData(std::string folderName)
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();

    // Cria pasta principal do mod
    CreateFolder(dataRootPath + "/mods/data/" + folderName + "/");

    this->modFolderPath = dataRootPath + "/mods/data/" + folderName + "/";
}

std::string ModData::GetFileFromAssets(const std::string& localPath)
{
    return modFolderPath + "/assets/" + localPath;
}

std::string ModData::GetFile(const std::string& localPath)
{
    // Retorna o caminho completo de um arquivo dentro da pasta do mod
    return modFolderPath + "/" + localPath;
}

std::string ModData::GetFileFromMenuSZK(const std::string& localPath)
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();
    std::string menuSZKPath = dataRootPath + "/mods/data/" + "menuSZK" + "/";

    return menuSZKPath + localPath;
}

void ModData::LoadSettings()
{
    auto pathSettings = GetFile("settings.ini");

    if (!file_exists(pathSettings)) return;

    IniReaderWriter ini;
    ini.LoadFromFile(pathSettings);

    CHASE_VEHICLE_MAX_SPEED = ini.GetInt("settings", "chase_vehicle_max_speed", CHASE_VEHICLE_MAX_SPEED);
    CHASE_POLICE_MAX_SPEED = ini.GetInt("settings", "chase_police_max_speed", CHASE_POLICE_MAX_SPEED);
    CHASE_MIN_TIME_TO_SURRENDER = ini.GetInt("settings", "chase_minimum_time_to_surrender", CHASE_MIN_TIME_TO_SURRENDER);
    g_secondsBetweenCallouts = ini.GetInt("settings", "seconds_between_callouts", g_secondsBetweenCallouts);
    CREATE_INJURED_PED = ini.GetBool("settings", "create_injured_ped", CREATE_INJURED_PED);
    DISABLE_CALLOUTS_RADIO_SOUND = ini.GetBool("settings", "disable_callouts_radio_sound", DISABLE_CALLOUTS_RADIO_SOUND);

    CHANCE_BEEING_WANTED_BY_JUSTICE = ini.GetDouble("chances", "chance_of_beeing_wanted_by_justice", CHANCE_BEEING_WANTED_BY_JUSTICE);
    CHANCE_RUNNING_AWAY_WHEN_VEHICLE_IRREGULAR =
        ini.GetDouble("chances", "chance_of_suspect_running_away_when_vehicle_is_irregular", CHANCE_RUNNING_AWAY_WHEN_VEHICLE_IRREGULAR);
    CHANCE_CRIMINAL_KILL_COPS = ini.GetDouble("chances", "chance_of_criminal_try_to_kill_cops_when_pulled_over", CHANCE_CRIMINAL_KILL_COPS);

    g_widgetsStartPosition.x = ini.GetDouble("widgets", "position_x", g_widgetsStartPosition.x);
    g_widgetsStartPosition.y = ini.GetDouble("widgets", "position_y", g_widgetsStartPosition.y);
}

void ModData::CreateFolder(const std::string& path)
{
    // Cria pasta se não existir
    mkdir(path.c_str(), 0777);
}