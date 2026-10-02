#pragma once

#include "../pch.h"

struct PoliceVehicleData
{
    std::string folderPath = "";
    std::string name = "";
    int vehicleModelId;
    int skinModelId;
    int occupants = 2;
    float chance = 1.0f;

    bool trunkDisabled = false;
    CVector trunkOffset = CVector(0, -4.00f, 0);
    CVector trunkSeatPosition = CVector(0.50f, -1.80f, 0.90f);
};