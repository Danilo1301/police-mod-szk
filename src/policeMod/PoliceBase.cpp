#include "PoliceBase.h"

#include "opcodeCaller/CleoFunctions.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "Peds.h"
#include "Partners.h"
#include "Checkpoint.h"
#include "Trunk.h"
#include "src/globals.h"
#include "src/utils/utils.h"

PoliceBase::PoliceBase()
{
    leaveCriminalCheckpoint = Checkpoints::CreateCheckpoint(CVector(0, 0, 0));
    leaveCriminalCheckpoint->onEnterCheckpoint = []()
    {
        auto playerActor = GetPlayerActor();

        auto vehicleRef = GetVehiclePedIsUsing(playerActor);

        if (vehicleRef <= 0)
        {
            BottomMessage::SetMessage(TT("not_in_a_vehicle"), 2000);
            return;
        }

        auto vehicle = Vehicles::GetVehicle(vehicleRef);

        std::vector<Ped*> pedsEscorted;

        auto passengers = vehicle->GetCurrentPassengers();
        for (auto passengerRef : passengers)
        {
            auto ped = Peds::GetPed(passengerRef);

            if (ped->flags.beeingEscorted) { pedsEscorted.push_back(ped); }
        }

        vehicle->trunk->CheckForNulls();

        bool hasPedsInTrunk = vehicle->trunk->GetPedsInTrunk().size() > 0;

        if (pedsEscorted.size() == 0 && !hasPedsInTrunk)
        {
            BottomMessage::SetMessage(TT("not_transporting_in_vehicle"), 2000);
            return;
        }

        int numSuspects = 0;

        for (auto ped : pedsEscorted)
        {
            numSuspects++;

            ped->LeaveCar();

            WAIT(3000, [ped]() { ped->QueueDestroy(); });
        }

        auto pedsInTrunkCopy = vehicle->trunk->GetPedsInTrunk();

        for (auto pedRef : pedsInTrunkCopy)
        {
            numSuspects++;
            vehicle->trunk->RemovePed(pedRef);

            auto ped = Peds::GetPed(pedRef);

            WAIT(2000, [ped]() { ped->QueueDestroy(); });
        }

        if (numSuspects == 1) { BottomMessage::SetMessage(TT("suspect_placed_at_justice"), 3000); }
        else
        {
            BottomMessage::SetMessage(TT("suspects_placed_at_justice", pedsEscorted.size()), 3000);
        }

        //BottomMessage::AddMessage("Recompensa: ~g~R$ 0", 3000);
    };

    getPartnerCheckpoint = Checkpoints::CreateCheckpoint(CVector(0, 0, 0));
    getPartnerCheckpoint->onEnterCheckpoint = []()
    {
        auto vehicleRef = GetVehiclePedIsUsing(GetPlayerActor());
        if (vehicleRef > 0)
        {
            BottomMessage::SetMessage(GetTranslatedText("error_cant_be_inside_vehicle"), 3000);
            return;
        }

        Partners::CreateSpawnPartnerMenu();
    };
}

void PoliceBase::Init()
{
    menuSZK->CreateBlip(texturePoliceDP, basePosition, 50.0f);
}

void PoliceBase::Update()
{
    auto playerActor = GetPlayerActor();
    auto vehicleRef = GetVehiclePedIsUsing(playerActor);

    auto currentPosition = vehicleRef > 0 ? GetCarPosition(vehicleRef) : GetPlayerPosition();

    leaveCriminalCheckpoint->CheckEntered(currentPosition);
    getPartnerCheckpoint->CheckEntered(currentPosition);
}

void PoliceBase::OnPostDrawRadar()
{
    //auto position = basePosition;

    //menuSZK->DrawTextureOnRadar(texturePoliceDP, position, CRGBA(255, 255, 255), 50.0f);
}

void PoliceBase::OnDraw()
{
    auto position = basePosition;

    DrawTextureOnWorld(texturePoliceDP, position, CRGBA(255, 255, 255), CVector2D(100, 100));
}