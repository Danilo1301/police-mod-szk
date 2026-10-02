#pragma once

#include "../pch.h"

#include "Vehicle.h"

class Chase
{
  public:
    static std::vector<Vehicle *> vehiclesInChase;

    static void Update();

    static void StartChaseWithVehicle(Vehicle *vehicle);
    static void AbortChase();
    static bool IsVehicleRunningFromCops(Vehicle *vehicle);
};