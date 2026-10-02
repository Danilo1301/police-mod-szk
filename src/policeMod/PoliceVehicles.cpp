#include "PoliceVehicles.h"
#include "../utils/iniReaderWriter.hpp"
#include "BackupUnits.h"

std::vector<PoliceVehicleData> PoliceVehicles::vehicles;
std::vector<PoliceVehicleData> PoliceVehicles::helicopters;

void PoliceVehicles::Initialize() {
    std::string mainFolder = modData->GetFile("data/police_vehicles/vehicles");
    std::string heliFolder = modData->GetFile("data/police_vehicles/helicopters");

    LoadVehiclesFromFolder(mainFolder, vehicles);
    LoadVehiclesFromFolder(heliFolder, helicopters);
}

void PoliceVehicles::LoadVehiclesFromFolder(const std::string& folder, std::vector<PoliceVehicleData>& outVector) {
    outVector.clear();

    for (auto& entry : std::filesystem::directory_iterator(folder)) {
        if (!entry.is_directory()) continue;

        std::string folderPath = entry.path().string();

        std::string backupIniPath = folderPath + "/backup.ini";

        if (!std::filesystem::exists(backupIniPath)) continue;

        PoliceVehicleData unit;
        unit.folderPath = folderPath;

        {
            IniReaderWriter ini;
            ini.LoadFromFile(backupIniPath);

            unit.name = ini.Get("backup", "name", "Unidade sem nome");
            unit.vehicleModelId = ini.GetInt("backup", "vehicle_model_id", -1);
            unit.skinModelId = ini.GetInt("backup", "ped_skin_id", -1);
            unit.occupants = ini.GetInt("backup", "occupants", 1);
            unit.chance = (float)ini.GetDouble("backup", "chance", 1.00);
        }

        if (unit.vehicleModelId == -1 || unit.skinModelId == -1) continue;

        std::string trunkIniPath = folderPath + "/trunk.ini";

        if (std::filesystem::exists(trunkIniPath)) {
            IniReaderWriter ini;
            ini.LoadFromFile(trunkIniPath);

            unit.trunkDisabled = ini.GetBool("trunk", "disabled", unit.trunkDisabled);
            unit.trunkOffset = ini.GetCVector("trunk", "offset", unit.trunkOffset);
            unit.trunkSeatPosition = ini.GetCVector("seat", "position", unit.trunkSeatPosition);
        }

        outVector.push_back(unit);
    }
}