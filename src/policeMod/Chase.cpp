#include "Chase.h"

#include "opcodeCaller/CleoFunctions.h"
#include "Vehicles.h"
#include "Peds.h"
#include "Criminals.h"
#include "ModelLoader.h"

std::vector<Vehicle*> Chase::vehiclesInChase;

void Chase::Update()
{
    // Remove veículos inválidos
    vehiclesInChase.erase(
        std::remove_if(vehiclesInChase.begin(), vehiclesInChase.end(), [](Vehicle* vehicle) { return !CAR_DEFINED(vehicle->ref); }),
        vehiclesInChase.end());
}

void Chase::StartChaseWithVehicle(Vehicle* vehicle)
{
    logger->Info("Chase: StartChaseWithVehicle");

    vehiclesInChase.push_back(vehicle);

    auto oldDriver = Peds::GetPed(vehicle->GetCurrentDriver());

    if (oldDriver == nullptr) return;

    auto driverModelId = GET_ACTOR_MODEL(oldDriver->ref);

    REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION(oldDriver->ref, vehicle->ref);

    auto newDriverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(vehicle->ref, PedType::CivMale, driverModelId);
    auto driver = Peds::RegisterPed(newDriverRef);

    driver->CopyFrom(*oldDriver);

    oldDriver->DestroyImmediate();

    SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
    SET_CAR_TO_PSYCHO_DRIVER(vehicle->ref);
    SET_CAR_MAX_SPEED(vehicle->ref, CHASE_VEHICLE_MAX_SPEED);

    vehicle->SetOwners();
    vehicle->ShowBlip(COLOR_CRIMINAL);

    for (auto pedRef : vehicle->GetCurrentOccupants())
    {
        auto ped = Peds::GetPed(pedRef);

        ped->ShowBlip(COLOR_CRIMINAL);
        Criminals::AddCriminal(ped);

        if (ped->flags.willKillCops)
        {
            ModelLoader::AddModelToLoad(346);
            ModelLoader::LoadAll([ped]() { GIVE_ACTOR_WEAPON(ped->ref, 22, 100); });
        }
    }
}

void Chase::AbortChase()
{
    for (auto vehicle : vehiclesInChase)
    {
        vehicle->RemoveBlips();

        auto occupants = vehicle->GetCurrentOccupants();
        for (auto pedRef : occupants)
        {
            auto ped = Peds::GetPed(pedRef);
            Criminals::RemoveCriminal(ped);
        }
    }

    vehiclesInChase.clear();
}

bool Chase::IsVehicleRunningFromCops(Vehicle* vehicle)
{
    return std::find(vehiclesInChase.begin(), vehiclesInChase.end(), vehicle) != vehiclesInChase.end();
}