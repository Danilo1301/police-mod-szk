#include "Trunk.h"

#include "opcodeCaller/CleoFunctions.h"
#include "Peds.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "ModelLoader.h"
#include "PoliceVehicles.h"
#include "../utils/iniReaderWriter.hpp"

Trunk::Trunk(int vehicleRef)
{
    this->vehicleRef = vehicleRef;
}

void Trunk::AddPedToTrunk(int pedRef)
{
    pedsInside.push_back(pedRef);

    auto vehicle = Vehicles::GetVehicle(vehicleRef);
    auto seatPosition = vehicle->policeVehicleData->trunkSeatPosition;

    int index = pedsInside.size() - 1;

    float x = seatPosition.x;

    if (index % 2 == 1) // se for ímpar → invertido
        x = -x;

    //CLEAR_ACTOR_TASK(pedRef);
    //REMOVE_REFERENCES_TO_ACTOR(pedRef);

    logger->Info("put into turret");

    PUT_ACTOR_INTO_TURRET_ON_CAR(pedRef, vehicleRef, x, seatPosition.y, seatPosition.z, 0, 0.0f, 0);

    auto ped = Peds::GetPed(pedRef);

    logger->Info("play anim");

    //ped->ClearAnim();
    ped->SetAnim("cower", "PED");
    ped->flags.showWidget = false;

    BottomMessage::SetMessage("Colocado no porta-malas", 2000);
}

void Trunk::RemoveAllPeds()
{
    auto pedsRefCopy = pedsInside;

    for (auto pedRef : pedsRefCopy) { RemovePed(pedRef); }
}

void Trunk::RemovePed(int pedRef)
{
    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    // Procurar o ped no vetor
    auto it = std::find(pedsInside.begin(), pedsInside.end(), pedRef);
    if (it == pedsInside.end()) return; // não existe no vetor

    // Lógica de remoção física do ped
    REMOVE_ACTOR_FROM_TURRET_MODE(pedRef);

    auto ped = Peds::GetPed(pedRef);
    ped->ClearAnim();

    auto newPosition = GetCarPositionWithOffset(vehicleRef, vehicle->trunkOffset);
    ped->SetPosition(newPosition);

    BottomMessage::SetMessage("Retirado do porta-malas", 2000);

    // Remover do vetor
    pedsInside.erase(it);
}

void Trunk::CreatePreviewPeds()
{
    logger->Info("CreatePreviewPeds");

    auto vehicleRefCopy = vehicleRef;

    ModelLoader::AddModelToLoad(193);
    ModelLoader::LoadAll(
        [vehicleRefCopy]()
        {
            auto playerActor = GetPlayerActor();
            auto position = GetPedPositionWithOffset(playerActor, CVector(0, 0, 10));

            auto ped1Handle = CREATE_ACTOR_PEDTYPE(PedType::Special, 193, position.x, position.y, position.z);
            Peds::RegisterPed(ped1Handle);

            auto ped2Handle = CREATE_ACTOR_PEDTYPE(PedType::Special, 193, position.x, position.y, position.z);
            Peds::RegisterPed(ped1Handle);

            auto vehicle = Vehicles::GetVehicle(vehicleRefCopy);

            if (vehicle) { BottomMessage::SetMessage("vehicle ok", 3000); }
            else
            {
                BottomMessage::SetMessage("vehicle not ok", 3000);
            }

            WAIT(20, [vehicle, ped1Handle]() { vehicle->trunk->AddPedToTrunk(ped1Handle); });

            WAIT(50, [vehicle, ped2Handle]() { vehicle->trunk->AddPedToTrunk(ped2Handle); });
        });
}

void Trunk::UpdatePreviewPeds()
{
    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    auto pedsInTrunk = vehicle->trunk->GetPedsInTrunk();

    if (pedsInTrunk.size() == 0) return;

    auto ped1Handle = pedsInTrunk[0];
    auto ped2Handle = pedsInTrunk[1];

    vehicle->trunk->RemoveAllPeds();

    vehicle->trunk->AddPedToTrunk(ped1Handle);
    vehicle->trunk->AddPedToTrunk(ped2Handle);
}

void Trunk::CheckForNulls()
{
    pedsInside.erase(std::remove_if(pedsInside.begin(),
                         pedsInside.end(),
                         [](auto ref)
                         {
                             auto ped = Peds::GetPed(ref);
                             return !Peds::IsValid(ped); // remove se for inválido
                         }),
        pedsInside.end());
}

void Trunk::OpenCustomizeMenu(int vehicleRef)
{
    auto vehicle = Vehicles::GetVehicle(vehicleRef);
    auto data = vehicle->policeVehicleData;

    auto window = CreatePM_Window(GetTranslatedText("window_customize_trunk"), "Trunk");

    window->onClose->Add(
        [data, vehicle]()
        {
            auto unit = data;
            std::string trunkIniPath = unit->folderPath + "/trunk.ini";

            IniReaderWriter ini;
            ini.SetBool("trunk", "disabled", unit->trunkDisabled);
            ini.SetCVector("trunk", "offset", unit->trunkOffset);
            ini.SetCVector("seat", "position", unit->trunkSeatPosition);
            ini.SaveToFile(trunkIniPath);

            auto peds = vehicle->trunk->GetPedsInTrunk();

            vehicle->trunk->RemoveAllPeds();

            for (auto pedRef : peds) { Peds::GetPed(pedRef)->QueueDestroy(); }
        });

    const float minValue = -1000.0f;
    const float maxValue = 1000.0f;
    const float step = 0.1f;

    {
        auto item = window->AddFloatOptions("X", &data->trunkSeatPosition.x, minValue, maxValue, step);
        item->onValueChange->Add([vehicle]() { vehicle->trunk->UpdatePreviewPeds(); });
    }

    {
        auto item = window->AddFloatOptions("Y", &data->trunkSeatPosition.y, minValue, maxValue, step);
        item->onValueChange->Add([vehicle]() { vehicle->trunk->UpdatePreviewPeds(); });
    }

    {
        auto item = window->AddFloatOptions("Z", &data->trunkSeatPosition.z, minValue, maxValue, step);
        item->onValueChange->Add([vehicle]() { vehicle->trunk->UpdatePreviewPeds(); });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("close"), [window, data, vehicle]() { window->Close(); });
    }
}