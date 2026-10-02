#pragma once

#include "../../pch.h"
#include "../Ped.h"
#include "../Vehicle.h"

class AIPed;
class AIVehicle;

class AIController
{
  public:
    static std::map<Vehicle *, AIVehicle *> vehicleAIs;
    static std::map<Ped *, AIPed *> pedsAI;

    static void AddAIToVehicle(Vehicle *vehicle, AIVehicle *ai);
    static void RemoveAIsFromVehicle(Vehicle *vehicle);

    static void AddAIToPed(Ped *ped, AIPed *ai);
    static void RemoveAIsFromPed(Ped *ped);

    static void Update();
};