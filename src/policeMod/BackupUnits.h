#pragma once

#include "../pch.h"

#include "PoliceVehicles.h"
#include "Vehicle.h"
#include "audio/AudioCollection.h"

struct RoadName
{
    AudioVariationGroup *audioGroup;
    CVector position;
};

class BackupUnits
{
  public:
    static std::vector<Vehicle *> backupVehicles;

    static void Initialize();
    static void InitializeRoads();
    static void Update();
    static void OnPostDrawRadar();
    static void CheckIfVehiclesAreValid();
    static void SendQTH();
    static void PlayRoadName();
    static void SpawnBackupUnit(PoliceVehicleData *unit);
    static void SpawnRandomBackupUnit();
    static void SpawnHelicopterBackup();
    static void AddVehicleAsBackup(Vehicle *vehicle, bool recreatePeds);
    static PoliceVehicleData *GetRandomUnitByChance(std::vector<PoliceVehicleData> &units);
    static void OpenSpawnBackupMenu();
    static void SpawnMedicUnit(CVector position);
};