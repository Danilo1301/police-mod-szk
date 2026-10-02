#include "AIHelicopterCop.h"

#include "../BottomMessage.h"
#include "../Criminals.h"
#include "../Peds.h"
#include "../ScriptTask.h"
#include "../Vehicles.h"
#include "../opcodeCaller/CleoFunctions.h"
#include "src/globals.h"

AIHelicopterCop::~AIHelicopterCop()
{
}

void AIHelicopterCop::Start()
{
    // menuDebug->AddLine("AIHelicopterCop started");

    FindTarget();
    DoAction();
}

void AIHelicopterCop::Update()
{
    if (isLeaving) return;
    if (!ACTOR_DEFINED(pedRef)) return;

    // auto cop = Peds::GetPed(pedRef);
    // auto copVehicle = Vehicles::GetVehicle(cop->previousVehicle);

    timer_findTarget += g_deltaTime;

    if (timer_findTarget > 3000)
    {
        timer_findTarget = 0;

        FindTarget();
        DoAction();
    }
}

void AIHelicopterCop::FindTarget()
{
    logger->Info("AICop: FindTarget");

    targetCriminal = -1;
    timer_findTarget = 0;

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
        targetCriminal = nearestFree->ref;
    else if (nearestAny)
        targetCriminal = nearestAny->ref;
    // caso não tenha nenhum válido, targetPed já é -1
}

void AIHelicopterCop::DoAction()
{
    logger->Info("AIHelicopterCop: DoAction");

    auto cop = Peds::GetPed(pedRef);

    if (cop->isEnteringCar || cop->isLeavingCar) return;

    auto copVehicle = Vehicles::GetVehicle(cop->previousVehicle);

    // menuDebug->AddLine("~b~cop do action");

    if (targetCriminal > 0)
    {
        if (copVehicle) { HELI_FOLLOW(copVehicle->ref, targetCriminal, -1, 16.0f); }
    }
    else
    {
        if (copVehicle)
        {
            if (!isLeaving)
            {
                isLeaving = true;

                ScriptTask* taskLeave = new ScriptTask("task_heli_leave");
                taskLeave->onBegin = [copVehicle]()
                {
                    BottomMessage::SetMessage("O aguia esta saindo", 3000);

                    HELI_FLY_TO(copVehicle->ref, 0, 0, 200.0f, 200.0f, 200.0f);
                };
                taskLeave->onExecute = [copVehicle]()
                {
                    if (!Vehicles::IsValid(copVehicle)) return SCRIPT_CANCEL;

                    auto towPosition = GetCarPosition(copVehicle->ref);
                    auto distance = DistanceFromPed(GetPlayerActor(), towPosition);

                    if (distance > 150.0f) return SCRIPT_SUCCESS;

                    return SCRIPT_KEEP_GOING;
                };
                taskLeave->onComplete = [copVehicle]()
                {
                    // on complete

                    BottomMessage::SetMessage("O aguia sumiu", 3000);

                    if (Vehicles::IsValid(copVehicle)) { copVehicle->QueueDestroy(true); }
                };
                taskLeave->Start();
            }
        }
    }
}