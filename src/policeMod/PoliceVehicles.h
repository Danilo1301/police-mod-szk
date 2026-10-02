#pragma once

#include "../pch.h"

#include "PoliceVehicleData.h"
#include "Vehicle.h"

class PoliceVehicles
{
  public:
    static std::vector<PoliceVehicleData> vehicles;
    static std::vector<PoliceVehicleData> helicopters;

    static void Initialize();

    static void LoadVehiclesFromFolder(const std::string &folder, std::vector<PoliceVehicleData> &outVector);
};

inline bool IsModelAPoliceHelicopter(int modelId)
{
    return std::any_of(PoliceVehicles::helicopters.begin(), PoliceVehicles::helicopters.end(),
        [modelId](const PoliceVehicleData &u) { return u.vehicleModelId == modelId; });
}

inline bool IsModelAPoliceVehicle(int modelId)
{
    return std::any_of(PoliceVehicles::vehicles.begin(), PoliceVehicles::vehicles.end(),
        [modelId](const PoliceVehicleData &u) { return u.vehicleModelId == modelId; });
}

inline PoliceVehicleData *GetPoliceVehicleDataByModelId(int modelId)
{
    for (auto &u : PoliceVehicles::vehicles)
    {
        if (u.vehicleModelId == modelId)
            return &u;
    }
    for (auto &u : PoliceVehicles::helicopters)
    {
        if (u.vehicleModelId == modelId)
            return &u;
    }
    return nullptr;
}