#pragma once

#include "../pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Escort
{
  public:
    static void Update();
    static void OpenEscortWindow(Ped *ped);
    static void OpenCarryingPedOptions(Ped *ped);
    static void EscortPed(Ped *ped);
    static void OnPlayerEnterVehicle();
    static bool IsEscortingSomeone();
    static bool IsCarryingSomeone();
    static bool IsPedBeeingCarried(Ped *ped);
    static Vehicle *GetClosestOpenTrunk();
};