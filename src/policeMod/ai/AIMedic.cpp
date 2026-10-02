#include "AIMedic.h"

#include "../BottomMessage.h"
#include "../Criminals.h"
#include "../Peds.h"
#include "../ScriptTask.h"
#include "../Vehicles.h"
#include "../opcodeCaller/CleoFunctions.h"
#include "AIController.h"

AIMedic::~AIMedic()
{
}

void AIMedic::Start()
{
    logger->Info("AIMedic: Start");
    // menuDebug->AddLine("AIMedic started");

    g_onPedLeaveVehicle->Add(
        [this](int pedRef)
        {
            if (pedRef == this->pedRef) DoAction();
        });

    g_onPedEnterVehicle->Add(
        [this](int enteredPedRef)
        {
            if (enteredPedRef == this->pedRef)
            {
                DoAction();

                for (auto r : Vehicles::GetVehicle(GetVehiclePedIsUsing(enteredPedRef))->GetCurrentOccupants())
                {
                    if (r == enteredPedRef) continue;

                    auto occupant = Peds::GetPed(r);
                    auto ai = (AIMedic*)AIController::pedsAI[occupant];
                    ai->DoAction();
                }
            }
        });

    auto medic = Peds::GetPed(pedRef);

    if (medic->IsDriver())
    {
        auto vehicle = Vehicles::GetVehicle(medic->GetCurrentCar());

        ScriptTask* taskDrive = new ScriptTask("drive");
        taskDrive->onBegin = [this, vehicle]()
        {
            SET_CAR_MAX_SPEED(vehicle->ref, 20.0f);
            SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
            CAR_DRIVE_TO(vehicle->ref, targetPosition.x, targetPosition.y, targetPosition.z);
            ENABLE_CAR_SIREN(vehicle->ref, true);
        };
        taskDrive->onExecute = [this, vehicle]()
        {
            if (!Vehicles::IsValid(vehicle)) return SCRIPT_CANCEL;

            auto distance = DistanceFromVehicle(vehicle->ref, targetPosition);

            if (distance < 10.0f) return SCRIPT_SUCCESS;

            return SCRIPT_KEEP_GOING;
        };
        taskDrive->onCancel = []() {
            
        };
        taskDrive->onComplete = [vehicle]() { vehicle->MakeOccupantsLeave(); };
        taskDrive->Start();
    }
}

void AIMedic::Update()
{
    if (!ACTOR_DEFINED(pedRef)) return;

    auto medic = Peds::GetPed(pedRef);

    auto vehicle = Vehicles::GetVehicle(medic->previousVehicle);

    if (vehicle)
    {
        if (AllMedicsAreDone(vehicle->ref))
        {
            if (!medic->IsInAnyCar())
            {
                if (!medic->isEnteringCar)
                {
                    logger->Info("AIMedic: fazendo todo mundo entrar");

                    vehicle->MakeOwnersEnter();
                }
            }

            if (vehicle->IsAllOwnersInside())
            {
                if (!vehicle->flags.isDrivingAway)
                {
                    vehicle->flags.isDrivingAway = true;

                    logger->Info("AIMedic: todos dentro, indo embora");

                    auto driver = Peds::GetPed(vehicle->GetCurrentDriver());
                    driver->StartDrivingRandomly();

                    REMOVE_REFERENCES_TO_CAR(vehicle->ref);
                }
            }
        }
    }
}

void AIMedic::FindTarget()
{
    logger->Info("AIMedic: FindTarget");

    targetPedRef = -1;
    targetPeds.clear();

    auto medic = Peds::GetPed(pedRef);

    CVector medicPos = medic->GetPosition(); // posição do médico
    float closestDist = 50.0f;               // limite máximo
    int closestPed = -1;

    auto peds = Peds::GetPedsMap();
    for (auto& pair : peds)
    {
        auto ped = pair.second;

        // Ignorar ped inválido
        if (!ped || ped->ref < 0) continue;

        // Tem que estar inconsciente
        if (!ped->flags.isInconcious) continue;

        // Ignorar peds que já estão sendo tratados
        if (ped->flags.isBeingTreated) continue;

        // Distância
        CVector pos = ped->GetPosition();
        float dist = distanceBetweenPoints(medicPos, pos);

        // É o mais próximo encontrado?
        if (dist < closestDist)
        {
            closestDist = dist;
            closestPed = ped->ref;

            targetPeds.push_back(ped->ref);
        }
    }

    targetPedRef = closestPed;
}

void AIMedic::DoAction()
{
    // menuDebug->AddLine("AIMedic: DoAction");

    auto medic = Peds::GetPed(pedRef);

    FindTarget();

    if (!medic->IsInAnyCar())
    {
        if (targetPedRef > 0)
        {
            if (medic->flags.targetPed <= 0)
            {
                // menuDebug->AddLine("AIMedic: indo socorrer");
                medic->flags.targetPed = targetPedRef;

                AproachPed(Peds::GetPed(targetPedRef));
            }
        }
        else
        {
            medic->flags.hasDoneWork = true;
        }
    }
}

void AIMedic::AproachPed(Ped* deadPed)
{
    deadPed->flags.isBeingTreated = true;

    auto medic = Peds::GetPed(pedRef);

    ScriptTask* taskAproach = new ScriptTask("aproach");
    taskAproach->SetStartAgainIfTimeout(2000);
    taskAproach->onBegin = [medic, deadPed]()
    {
        // menuDebug->AddLine("AIMedic: andando");
        CLEAR_ACTOR_TASK(medic->ref);
        TASK_FOLLOW_FOOTSTEPS(medic->ref, deadPed->ref);
    };
    taskAproach->onExecute = [medic, deadPed]()
    {
        if (!Peds::IsValid(medic)) return SCRIPT_CANCEL;
        if (!Peds::IsValid(deadPed)) return SCRIPT_CANCEL;

        auto distance = distanceBetweenPoints(medic->GetPosition(), deadPed->GetPosition());
        if (distance < 1.5f) return SCRIPT_SUCCESS;

        return SCRIPT_KEEP_GOING;
    };
    taskAproach->onCancel = []()
    {
        // menuDebug->AddLine("AIMedic: cancelou");
    };
    taskAproach->onComplete = [this, deadPed]()
    {
        // menuDebug->AddLine("AIMedic: chegou no ped morto");
        RessurectPed(deadPed);
    };
    taskAproach->Start();
}

void AIMedic::RessurectPed(Ped* deadPed)
{
    auto medic = Peds::GetPed(pedRef);

    auto tpPosition = GetPedPositionWithOffset(deadPed->ref, CVector(0, -1.0f, 0));

    SET_CHAR_COORDINATES(medic->ref, tpPosition.x, tpPosition.y, tpPosition.z);

    PERFORM_ANIMATION_AS_ACTOR(medic->ref, "CPR", "MEDIC", 8.0f, 0, 0, 0, 0, -1);

    WAIT(6000, [deadPed]() { deadPed->Reanimate(); });

    WAIT(6000 + 500,
        [this, medic]()
        {
            medic->flags.targetPed = -1;
            // menuDebug->AddLine("AIMedic: flag resetada, livre");
            DoAction();
        });
}

bool AIMedic::AllMedicsAreDone(int vehicleRef)
{
    auto owners = Vehicles::GetVehicle(vehicleRef)->GetOwners();

    for (auto ref : owners)
    {
        auto medic = Peds::GetPed(ref);

        if (!medic->flags.hasDoneWork) return false;
    }

    return true;
}