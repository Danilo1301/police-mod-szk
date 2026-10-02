#pragma once

#include "../pch.h"

#include "Vehicle.h"

class Vehicles
{
  private:
    static std::map<int, Vehicle *> vehicles;

  public:
    static void AddVehicle(int ref, void *ptr);
    static Vehicle *RegisterVehicle(int ref);
    static void RemoveVehicle(int ref);
    static bool IsValid(Vehicle *veh);
    static Vehicle *GetVehicle(int ref);
    static std::map<int, Vehicle *> GetVehiclesMap();
    static void Update();

    static std::vector<Vehicle *> GetAllCarsInSphere(CVector position, float radius);
    static Vehicle *GetClosestVehicle(CVector sphereCenter, CVector targetPosition, float radius);
    static Vehicle *GetClosestVehicleNotPlayer(CVector sphereCenter, CVector targetPosition, float radius);
};