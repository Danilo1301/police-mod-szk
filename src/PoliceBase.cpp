#include "PoliceBase.h"

#include "CleoFunctions.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "Peds.h"
#include "Partners.h"

PoliceBase::PoliceBase()
{
    leaveCriminalCheckpoint = Checkpoints::CreateCheckpoint(CVector(0, 0, 0));
    leaveCriminalCheckpoint->onEnterCheckpoint = []() {
        auto playerActor = GetPlayerActor();

        auto vehicleRef = GetVehiclePedIsUsing(playerActor);

        if(vehicleRef <= 0)
        {
            BottomMessage::SetMessage("~r~Voce nao esta em um veiculo", 2000);
            return;
        }

        auto vehicle = Vehicles::GetVehicle(vehicleRef);

        std::vector<Ped*> pedsEscorted;

        auto passengers = vehicle->GetCurrentPassengers();
        for(auto passengerRef : passengers)
        {
            auto ped = Peds::GetPed(passengerRef);

            if(ped->flags.beeingEscorted)
            {
                pedsEscorted.push_back(ped);
            }
        }

        vehicle->trunk->CheckForNulls();

        bool hasPedsInTrunk = vehicle->trunk->GetPedsInTrunk().size() > 0;

        if(pedsEscorted.size() == 0 && !hasPedsInTrunk)
        {
            BottomMessage::SetMessage("~r~Voce nao esta transportando nenhum suspeito neste veiculo", 2000);
            return;
        }

        int numSuspects = 0;

        for(auto ped : pedsEscorted)
        {
            numSuspects++;

            ped->LeaveCar();

            WAIT(3000, [ped]() {
                ped->QueueDestroy();
            });
        }

        auto pedsInTrunkCopy = vehicle->trunk->GetPedsInTrunk();

        for(auto pedRef : pedsInTrunkCopy)
        {
            numSuspects++;
            vehicle->trunk->RemovePed(pedRef);

            auto ped = Peds::GetPed(pedRef);

            WAIT(2000, [ped]() {
                ped->QueueDestroy();
            });
        }

        if(numSuspects == 1)
        {
            BottomMessage::SetMessage("O suspeito ficou a ~y~disposicao da justica", 3000);
        } else {
            BottomMessage::SetMessage("Os " + std::to_string(pedsEscorted.size()) + " suspeitos ficaram a ~y~disposicao da justica", 3000);
        }

        //BottomMessage::AddMessage("Recompensa: ~g~R$ 0", 3000);
    };

    getPartnerCheckpoint = Checkpoints::CreateCheckpoint(CVector(0, 0, 0));
    getPartnerCheckpoint->onEnterCheckpoint = []() {

        auto vehicleRef = GetVehiclePedIsUsing(GetPlayerActor());
        if(vehicleRef > 0)
        {
            BottomMessage::SetMessage(GetTranslatedText("error_cant_be_inside_vehicle"), 3000);
            return;
        }

        Partners::CreateSpawnPartnerMenu();
    };
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
    auto position = basePosition;
        
    menuSZK->DrawTextureOnRadar(texturePoliceDP, position, CRGBA(255, 255, 255), 50.0f);
}

void PoliceBase::OnDraw()
{
    auto position = basePosition;

    menuSZK->DrawTextureOnWorld(texturePoliceDP, position, CRGBA(255, 255, 255), CVector2D(100, 100));
}