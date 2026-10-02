#include "AICop.h"

#include "../../globals.h"
#include "../Criminals.h"
#include "../Peds.h"
#include "../Vehicles.h"
#include "../opcodeCaller/CleoFunctions.h"

AICop::~AICop()
{
}

void AICop::Start()
{
    logger->Info("AICop: Start");

    g_onPedLeaveVehicle->Add(
        [this](int pedRef)
        {
            logger->Info("Ped left vehicle");

            if (pedRef == this->pedRef) { DoAction(); }
        });
}

void AICop::Update()
{
    // logger->Info("AICop: Update");

    if (!ACTOR_DEFINED(pedRef)) return;

    auto cop = Peds::GetPed(pedRef);

    auto copVehicle = Vehicles::GetVehicle(cop->previousVehicle);

    if (copVehicle)
    {
        auto distanceToPlayer = distanceBetweenPoints(copVehicle->GetPosition(), GetPlayerPosition());

        if (distanceToPlayer > 300)
        {
            logger->Info("AICop: Destroy vehicle (too far)");

            for (auto owner : copVehicle->GetOwners()) { QueueDestroyPed(owner); }

            QueueDestroyVehicle(copVehicle->ref);

            return;
        }
    }

    if (leavingScene) return;

    int dt = g_deltaTime;

    findTargetTimer += dt;

    if (findTargetTimer >= 3000)
    {
        findTargetTimer = 0;
        FindTarget();
    }

    if (!ACTOR_DEFINED(targetPed)) { targetPed = -1; }

    if (targetPed != prevTargetPed)
    {
        DoAction();
        prevTargetPed = targetPed;
    }

    if (copVehicle)
    {
        auto drivingToPed = copVehicle->flags.drivingToPed;

        if (drivingToPed != -1 && ACTOR_DEFINED(drivingToPed))
        {
            driveToTimer += g_deltaTime;

            if (driveToTimer > 3000)
            {
                driveToTimer = 0;

                // it only resets the flag if the ped is on foot
                if (IS_CHAR_IN_ANY_CAR(drivingToPed) == false) { copVehicle->flags.drivingToPed = -1; }
            }
        }
    }

    if (targetPed != -1)
    {
        auto criminal = Peds::GetPed(targetPed);
        auto criminalPosition = criminal->GetPosition();

        auto distanceToCriminal = distanceBetweenPoints(cop->GetPosition(), criminalPosition);

        if (distanceToCriminal > 30.0f)
        {
            if (copVehicle)
            {
                copVehicle->ValidateOwners();

                if (!copVehicle->IsAllOwnersInside())
                {
                    copVehicle->flags.drivingToPed = -1;

                    if (!copVehicle->IsAnyOwnersLeavingOrEnteringCar())
                    {
                        logger->Info("AICop: Entering vehicle");

                        copVehicle->MakeOwnersEnter();
                    }
                }

                if (copVehicle->IsAllOwnersInside())
                {
                    if (copVehicle->flags.drivingToPed != targetPed && cop->IsDriver())
                    {
                        copVehicle->flags.drivingToPed = targetPed;

                        logger->Info("Cop drive to target");

                        ENABLE_CAR_SIREN(copVehicle->ref, true);
                        SET_CAR_TRAFFIC_BEHAVIOUR(copVehicle->ref, DrivingMode::AvoidCars);
                        SET_CAR_MAX_SPEED(copVehicle->ref, CHASE_POLICE_MAX_SPEED);

                        if (criminal->IsInAnyCar())
                        {
                            auto criminalCarRef = criminal->GetCurrentCar();

                            CAR_FOLLOW_CAR(copVehicle->ref, criminalCarRef, 7.0f);
                        }
                        else
                        {
                            CAR_DRIVE_TO(copVehicle->ref, criminalPosition.x, criminalPosition.y, criminalPosition.z);
                        }
                    }
                }
            }
        }

        if (distanceToCriminal < 20.0f)
        {
            bool criminalCarIsSlow = false;
            if (criminal->IsInAnyCar()) { criminalCarIsSlow = CAR_SPEED(criminal->GetCurrentCar()) < 2.0f; }

            if (copVehicle)
            {
                bool canLeave = !criminal->IsInAnyCar() || criminalCarIsSlow;

                if (canLeave)
                {
                    copVehicle->ValidateOwners();

                    if (copVehicle->GetCurrentOccupants().size() > 0 && !copVehicle->IsAnyOwnersLeavingOrEnteringCar())
                    {
                        if (cop->IsDriver())
                        {
                            logger->Info("AICop: Cops leaving vehicle");

                            copVehicle->MakeOccupantsLeave();
                        }
                    }
                }
            }
        }
    }

    if (copVehicle)
    {
        auto criminalsCount = Criminals::GetCriminals()->size();

        // if(criminalsCount > 0)
        // {
        //     leavingScene = false;
        // }
        if (criminalsCount == 0)
        {
            if (!leavingScene)
            {
                leavingScene = true;

                logger->Info("AICop: Cops leaving scene");

                copVehicle->MakeOwnersEnter();
                copVehicle->HideBlip();

                CleoFunctions::AddWaitForFunction(
                    "cops_entercar_to_leave",
                    [copVehicle]()
                    {
                        if (!Vehicles::IsValid(copVehicle)) return true;

                        if (copVehicle->IsAllOwnersInside()) return true;

                        return false;
                    },
                    [copVehicle]()
                    {
                        if (!Vehicles::IsValid(copVehicle)) return;

                        ENABLE_CAR_SIREN(copVehicle->ref, false);

                        auto driverRef = copVehicle->GetCurrentDriver();

                        if (driverRef > 0)
                        {
                            auto driver = Peds::GetPed(driverRef);
                            driver->StartDrivingRandomly();
                        }
                    });
            }
        }
    }
}

void AICop::FindTarget()
{
    logger->Info("AICop: FindTarget");

    targetPed = -1;
    findTargetTimer = 0;

    // garante que o próprio cop exista
    auto selfPed = Peds::GetPed(pedRef);
    if (!selfPed) return;

    auto criminals = *Criminals::GetCriminals();
    if (criminals.empty()) return;

    Ped* nearestFree = nullptr; // não surrender / não inconcious
    float nearestFreeDist = std::numeric_limits<float>::max();

    Ped* nearestAny = nullptr; // qualquer um (fallback)
    float nearestAnyDist = std::numeric_limits<float>::max();

    auto selfPos = selfPed->GetPosition();

    for (auto crim : criminals)
    {
        if (!crim) continue;

        // tenta obter o Ped do criminoso
        auto crimPed = Peds::GetPed(crim->ref);
        if (!crimPed) continue;

        float dist = distanceBetweenPoints(selfPos, crimPed->GetPosition());

        // mantem o mais próximo de todos (fallback)
        if (dist < nearestAnyDist)
        {
            nearestAnyDist = dist;
            nearestAny = crim;
        }

        // considera apenas os que NÃO estão surrender ou inconcious
        if (!crim->flags.hasSurrended && !crim->flags.isInconcious)
        {
            if (dist < nearestFreeDist)
            {
                nearestFreeDist = dist;
                nearestFree = crim;
            }
        }
    }

    // prefere o nearestFree; caso não exista, usa nearestAny
    if (nearestFree)
        targetPed = nearestFree->ref;
    else if (nearestAny)
        targetPed = nearestAny->ref;
    // caso não tenha nenhum válido, targetPed já é -1
}

void AICop::DoAction()
{
    logger->Info("AICop: DoAction");

    auto cop = Peds::GetPed(pedRef);

    if (cop->isEnteringCar || cop->isLeavingCar) return;

    if (targetPed != -1)
    {
        auto criminal = Peds::GetPed(targetPed);

        if (criminal)
        {
            // on foot
            if (!criminal->IsInAnyCar())
            {
                if (!cop->IsInAnyCar())
                {
                    if (criminal->flags.willSurrender)
                    {
                        TASK_FOLLOW_FOOTSTEPS(pedRef, targetPed);
                        AIM_AT_ACTOR(pedRef, targetPed, 8000);
                    }
                    else
                    {

                        if (criminal->flags.willKillCops) { KILL_ACTOR(pedRef, targetPed); }
                        else
                        {
                            TASK_FOLLOW_FOOTSTEPS(pedRef, targetPed);
                        }
                    }
                }
            }

            // on vehicle
            if (criminal->IsInAnyCar())
            {
                if (!cop->IsInAnyCar())
                {
                    if (criminal->flags.willSurrender)
                    {
                        TASK_FOLLOW_FOOTSTEPS(pedRef, targetPed);
                        AIM_AT_ACTOR(pedRef, targetPed, 8000);
                    }
                    else
                    {

                        if (criminal->flags.willKillCops) { KILL_ACTOR(pedRef, targetPed); }
                        else
                        {
                            TASK_FOLLOW_FOOTSTEPS(pedRef, targetPed);
                            AIM_AT_ACTOR(pedRef, targetPed, 8000);
                        }
                    }
                }
            }
        }
    }

    logger->Info("AICop: DoAction DONE");
}