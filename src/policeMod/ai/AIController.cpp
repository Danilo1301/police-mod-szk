#include "AIController.h"

#include "../Vehicles.h"
#include "../Peds.h"
#include "AIVehicle.h"
#include "AIPed.h"

std::map<Vehicle*, AIVehicle*> AIController::vehicleAIs;
std::map<Ped*, AIPed*> AIController::pedsAI;

void AIController::AddAIToVehicle(Vehicle* vehicle, AIVehicle* ai)
{
    if (!vehicle || !ai) return; // checagem básica

    ai->vehicleRef = vehicle->ref;

    // adiciona ou atualiza
    vehicleAIs[vehicle] = ai;
}

void AIController::RemoveAIsFromVehicle(Vehicle* vehicle)
{
    if (!vehicle) return;

    auto it = vehicleAIs.find(vehicle);
    if (it != vehicleAIs.end())
    {
        AIVehicle* ai = it->second;

        if (ai != nullptr) delete ai;

        vehicleAIs.erase(it); // remove do map
    }
}

void AIController::AddAIToPed(Ped* ped, AIPed* ai)
{
    if (!ped || !ai) return; // checagem básica

    ai->pedRef = ped->ref;

    // adiciona ou atualiza
    pedsAI[ped] = ai;
}

void AIController::RemoveAIsFromPed(Ped* ped)
{
    if (!ped) return;

    auto it = pedsAI.find(ped);
    if (it != pedsAI.end())
    {
        AIPed* ai = it->second;

        if (ai != nullptr) delete ai;

        pedsAI.erase(it); // remove do map
    }
}

void AIController::Update()
{
    // cópia rasa do mapa (shallow copy dos pares)
    {
        auto cachedAIs = vehicleAIs;

        for (const auto& [vehicle, ai] : cachedAIs)
        {
            // se ponteiro nulo ou veículo inválido -> remove do mapa original (se existir) e pula
            if (!Vehicles::IsValid(vehicle))
            {
                auto itOrig = vehicleAIs.find(vehicle);
                if (itOrig != vehicleAIs.end()) { RemoveAIsFromVehicle(vehicle); }
                continue;
            }

            // só atualiza se o par ainda existir no mapa original e apontar para o mesmo AI
            auto itOrig = vehicleAIs.find(vehicle);
            if (itOrig != vehicleAIs.end() && itOrig->second == ai)
            {
                ai->Update(); // dentro daqui o AI pode chamar RemoveAIsFromVehicle(...)
            }
        }
    }

    // cópia rasa do mapa (shallow copy dos pares)
    {
        auto cachedAIs = pedsAI;

        for (const auto& [ped, ai] : cachedAIs)
        {
            // se ponteiro nulo ou veículo inválido -> remove do mapa original (se existir) e pula
            if (!Peds::IsValid(ped))
            {
                auto itOrig = pedsAI.find(ped);
                if (itOrig != pedsAI.end()) { RemoveAIsFromPed(ped); }
                continue;
            }

            // só atualiza se o par ainda existir no mapa original e apontar para o mesmo AI
            auto itOrig = pedsAI.find(ped);
            if (itOrig != pedsAI.end() && itOrig->second == ai)
            {
                ai->Update(); // dentro daqui o AI pode chamar RemoveAIsFromVehicle(...)
            }
        }
    }
}