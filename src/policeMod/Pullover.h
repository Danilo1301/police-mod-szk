#pragma once

#include "../pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Pullover
{
  public:
    static void Update();

    static void OnClickWidget();

    static void TryPulloverFromVehicle();
    static void TryPulloverOnFoot();
    static void TryPulloverClosestVehicle();

    static void PulloverPed(Ped *ped);
    static void FreePed(Ped *ped);

    static void PulloverVehicle(Vehicle *vehicle);
    static void FreeVehicle(Vehicle *vehicle);

    static int FindAimingPed();

    static void OpenPedMenu(Ped *ped);
    static void OpenVehicleMenu(Vehicle *vehicle);

    static void CallTowTruck(Vehicle *vehicle);

    static void AskVehicleToMoveToTheRight(Vehicle *vehicle);
};