#pragma once

#include "../pch.h"

#include "Ped.h"
#include "Vehicle.h"

class DocsWindow
{
public:
    static void ShowRG(Ped* ped);
    static void ShowRGResults(Ped* ped);

    static void ShowVehicleVisualInfo(Vehicle* vehicle);
    static void ShowVehicleResults(Vehicle* vehicle, bool byPlate);

    static void ShowCRLV(Ped* ped, Vehicle* vehicle);

    static void ShowCNH(Ped* ped);

    static void ShowChassisResult(Vehicle* vehicle);
};