#include "ScriptTask.h"

#include "BottomMessage.h"
#include "ModelLoader.h"
#include "Peds.h"
#include "TopMessage.h"
#include "opcodeCaller/CleoFunctions.h"
#include "src/globals.h"

ScriptTask::ScriptTask(std::string name) { this->name = name; }

void ScriptTask::Start() {
    // menuDebug->AddLine("task : " + name + " : start");

    if (onBegin) onBegin();

    CleoFunctions::AddWaitForFunction(
        name,
        [this]() {
            timePassedToTimeout += g_deltaTime;
            totalTimeElapsed += g_deltaTime;

            if (timeout > 0) {
                if (timePassedToTimeout >= timeout) {
                    timePassedToTimeout = 0;
                    if (onBegin) onBegin();
                }
            }

            if (onExecute) {
                int result = onExecute();
                scriptStatus = result;
            } else {
                scriptStatus = SCRIPT_SUCCESS;
            }

            return scriptStatus != SCRIPT_KEEP_GOING;
        },
        [this]() {
            auto fn_onComplete = onComplete;
            auto fn_onCancel = onCancel;

            //menuDebug->AddLine("task : " + name + " : completed");

            if (deleteOnComplete) delete this;

            if (scriptStatus == SCRIPT_SUCCESS) {
                if (fn_onComplete) fn_onComplete();
            } else {
                if (fn_onCancel) fn_onCancel();
            }
        });
}

void ScriptTask::SetStartAgainIfTimeout(int timeout) { this->timeout = timeout; }

void ScriptTask::MakeVehicleLeave(int vehicleRef) {
    auto startPosition = GetCarPosition(vehicleRef);

    auto targetPosition = GET_CLOSEST_CAR_NODE(startPosition.x, startPosition.y + 1000, startPosition.z);

    SET_CAR_MAX_SPEED(vehicleRef, 20.0f);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicleRef, DrivingMode::StopForCarsIgnoreLights);
    CAR_DRIVE_TO(vehicleRef, targetPosition.x, targetPosition.y, targetPosition.z);
}

void ScriptTask::CreateTest() {
    // int pedModelId = 40;

    // ModelLoader::AddModelToLoad(pedModelId);

    // ModelLoader::LoadAll([pedModelId]() {
    //     auto playerActor = GetPlayerActor();
    //     auto playerPosition = GetPedPosition(playerActor);

    //     auto spawnPosition = STORE_PED_PATH_COORDS_CLOSEST_TO(playerPosition.x, playerPosition.y, playerPosition.z);
    //     auto targetPosition = spawnPosition;
    //     targetPosition.x += 10;

    //     auto pedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, pedModelId, spawnPosition.x, spawnPosition.y,
    //     spawnPosition.z); auto ped = Peds::RegisterPed(pedRef);

    //     //

    //     ScriptTask* taskWalkToAnotherPoint = new ScriptTask("test_walk");
    //     taskWalkToAnotherPoint->SetStartAgainIfTimeout(8000);

    //     taskWalkToAnotherPoint->onBegin = [pedRef, targetPosition]() {
    //         BottomMessage::SetMessage("Rodando task", 1000);
    //         TASK_GO_TO_COORD_ANY_MEANS(pedRef, targetPosition.x, targetPosition.y, targetPosition.z, 6, false);
    //     };
    //     taskWalkToAnotherPoint->onExecute = [taskWalkToAnotherPoint, pedRef, targetPosition]() {

    //         auto distance = DistanceFromPed(pedRef, targetPosition);

    //         menuDebug->AddLine("distance to " + CVectorToString(targetPosition) + " is " + std::to_string(distance));

    //         if(distance > 3.0f)
    //         {
    //             return false;
    //         }

    //         return true;
    //     };
    //     taskWalkToAnotherPoint->onComplete = [ped]() {

    //         BottomMessage::SetMessage("Tarefa completada", 3000);
    //         Objectives::ClearObjective();

    //     };
    //     taskWalkToAnotherPoint->Start();

    //     Objectives::SetObjective("Aguarde o ped chegar ao destino");
    // });
}