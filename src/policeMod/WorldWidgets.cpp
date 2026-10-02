#include "WorldWidgets.h"

#include "Peds.h"
#include "Vehicle.h"
#include "Vehicles.h"
#include "mod/logger.h"
#include "src/globals.h"

#include "Peds.h"
#include "Vehicles.h"

void UpdateWorldWidgets()
{
    auto peds = Peds::GetPedsMap();

    for (auto& pair : peds)
    {
        auto& ped = pair.second;

        if (!ped->worldWidget) continue;

        UpdateWorldWidget(ped->worldWidget);
    }

    auto vehicles = Vehicles::GetVehiclesMap();

    for (auto& pair : vehicles)
    {
        auto& vehicle = pair.second;

        if (!vehicle->worldWidget) continue;

        UpdateWorldWidget(vehicle->worldWidget);
    }
}

void UpdateWorldWidget(WorldWidget* wWidget)
{
    CVector worldPosition;
    bool valid = false;
    bool center = true;

    if (wWidget->attachToPed != -1)
    {
        auto ped = Peds::GetPed(wWidget->attachToPed);

        if (Peds::IsValid(ped))
        {
            worldPosition = ped->GetPosition();
            valid = true;
        }
    }
    else if (wWidget->attachToVehicle != -1)
    {
        auto vehicle = Vehicles::GetVehicle(wWidget->attachToVehicle);

        if (Vehicles::IsValid(vehicle))
        {
            worldPosition = vehicle->GetPosition();
            valid = true;
        }
    }

    if (!valid) return;

    auto screenPos = menuSZK->ConvertWorldToScreenCoords(worldPosition, true);

    if (center)
    {
        auto size = wWidget->widget->GetSize();
        screenPos.x -= size / 2;
        screenPos.y -= size / 2;
    }

    wWidget->widget->SetPosition(screenPos.x, screenPos.y);
}