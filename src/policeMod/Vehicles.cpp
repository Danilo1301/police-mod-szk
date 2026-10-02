#include "Vehicles.h"

#include "../hooks.h"
#include "mod/logger.h"
#include "opcodeCaller/CleoFunctions.h"

std::map<int, Vehicle*> Vehicles::vehicles;

void Vehicles::AddVehicle(int ref, void* ptr)
{
    if (vehicles.find(ref) != vehicles.end())
    {
        // já existe

        logger->Info("Vehicles: could not add vehicle. aready exists");
        return;
    }

    logger->Info("Vehicles: Adding vehicle");

    auto veh = new Vehicle(ref, ptr);
    vehicles[ref] = veh;

    logger->Info("Vehicles: vehicle added");
}

Vehicle* Vehicles::RegisterVehicle(int ref)
{
    if (vehicles.find(ref) != vehicles.end())
    {
        // já existe
        return NULL;
    }

    auto ptr = GetVehicleFromRef(ref);

    auto veh = new Vehicle(ref, ptr);
    vehicles[ref] = veh;

    return veh;
}

void Vehicles::RemoveVehicle(int ref)
{
    // verifica se existe
    auto it = vehicles.find(ref);
    if (it == vehicles.end()) return; // não existe, sai

    // pega o ped
    Vehicle* veh = it->second;

    // remove do map
    vehicles.erase(it);

    // deleta o objeto
    delete veh;
}

bool Vehicles::IsValid(Vehicle* veh)
{
    if (!veh) return false; // ponteiro nulo nunca é válido

    for (const auto& pair : vehicles)
    {
        if (pair.second == veh) // encontrou o ponteiro
        {
            return CAR_DEFINED(veh->ref);
        }
    }

    return false; // não encontrado
}

Vehicle* Vehicles::GetVehicle(int ref)
{
    if (ref < 0) return nullptr;

    auto it = vehicles.find(ref);
    if (it == vehicles.end()) return nullptr; // não existe

    return it->second; // existe, retorna
}

std::map<int, Vehicle*> Vehicles::GetVehiclesMap()
{
    return vehicles;
}

void Vehicles::Update()
{
    auto vehiclesCopy = vehicles;

    // percorre todos os peds no map
    for (auto& pair : vehiclesCopy)
    {
        Vehicle* veh = pair.second;

        if (!veh) continue;
        if (!CAR_DEFINED(veh->ref)) continue;

        // chama o update individual do ped
        veh->Update();
    }
}

std::vector<Vehicle*> Vehicles::GetAllCarsInSphere(CVector position, float radius)
{
    logger->Info("GetAllCarsInSphere: START");
    logger->Info("Vehicles count: %zu", vehicles.size());

    std::vector<Vehicle*> foundVehicles;

    for (auto pair : vehicles)
    {
        logger->Info("Processing vehicle");

        auto vehicle = pair.second;

        logger->Info("Vehicle ptr: %p", vehicle);

        if (!vehicle)
        {
            logger->Info("Vehicle is NULL");
            continue;
        }

        logger->Info("Vehicle ref: %d", vehicle->ref);

        auto vehiclePos = GetCarPosition(vehicle->ref);

        logger->Info("GetCarPosition OK");

        auto distance = distanceBetweenPoints(position, vehiclePos);

        logger->Info("Distance: %f", distance);

        if (distance <= radius)
        {
            foundVehicles.push_back(vehicle);
            logger->Info("Vehicle added");
        }
    }

    logger->Info("GetAllCarsInSphere: END, found = %zu", foundVehicles.size());

    return foundVehicles;
}

Vehicle* Vehicles::GetClosestVehicle(CVector sphereCenter, CVector targetPosition, float radius)
{
    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(sphereCenter, radius);

    Vehicle* closestCar = NULL;
    double closestDistance = 99999;

    for (size_t i = 0; i < vehicles.size(); i++)
    {
        auto vehicle = vehicles[i];
        auto vehiclePosition = GetCarPosition(vehicle->ref);

        auto distance = distanceBetweenPoints(vehiclePosition, targetPosition);

        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestCar = vehicle;
        }
    }

    if (!closestCar) return NULL;

    return closestCar;
}

Vehicle* Vehicles::GetClosestVehicleNotPlayer(CVector sphereCenter, CVector targetPosition, float radius)
{
    logger->Info("GetClosestVehicleNotPlayer: START");

    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(sphereCenter, radius);

    logger->Info("GetAllCarsInSphere OK");
    logger->Info("Vehicle count: %zu", vehicles.size());

    Vehicle* closestCar = NULL;
    double closestDistance = 99999;

    for (size_t i = 0; i < vehicles.size(); i++)
    {
        logger->Info("Vehicle index: %zu", i);

        auto vehicle = vehicles[i];

        if (!vehicle)
        {
            logger->Info("Vehicle is NULL");
            continue;
        }

        logger->Info("Vehicle pointer: %p", vehicle);
        logger->Info("Vehicle ref: %d", vehicle->ref);

        if (vehicle->IsPlayerInside())
        {
            logger->Info("IsPlayerInside: true");
            continue;
        }

        logger->Info("IsPlayerInside: false");

        auto vehiclePosition = GetCarPosition(vehicle->ref);

        logger->Info("GetCarPosition OK");

        auto distance = distanceBetweenPoints(vehiclePosition, targetPosition);

        logger->Info("Distance: %f", distance);

        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestCar = vehicle;

            logger->Info("New closest vehicle");
        }
    }

    logger->Info("Vehicle loop finished");

    if (!closestCar)
    {
        logger->Info("No closest vehicle");
        return NULL;
    }

    logger->Info("Closest vehicle found: %p", closestCar);

    return closestCar;
}