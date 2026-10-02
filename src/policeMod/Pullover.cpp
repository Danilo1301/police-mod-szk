#include "Pullover.h"

#include "Peds.h"
#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include "opcodeCaller/CleoFunctions.h"
#include "Criminals.h"
#include "ModelLoader.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "TopMessage.h"
#include "ScriptTask.h"
#include "Chase.h"
#include "RadioWindow.h"
#include "Escort.h"
#include "DocsWindow.h"
#include "audio/AudioCollection.h"
#include "RadioSounds.h"
#include "RGWindow.h"
#include "CNHWindow.h"
#include "../globals.h"
#include "Trunk.h"

int aimingPed = NO_PED_FOUND;

void CheckAimingPed()
{
    aimingPed = Pullover::FindAimingPed();
}

void Pullover::Update()
{
    int dt = g_deltaTime;

    static int accumulatorMs = 0;
    accumulatorMs += dt;

    if (accumulatorMs >= 300)
    {
        accumulatorMs = 0;
        CheckAimingPed();
    }
}

void Pullover::OnClickWidget()
{
    logger->Info("Clicked pullover widget");

    //g_debugOpcodes = true;

    int playerActor = GET_PLAYER_ACTOR(0);

    if (IS_CHAR_IN_ANY_CAR(playerActor))
    {
        logger->Info("TryPulloverFromVehicle");
        TryPulloverFromVehicle();
    }
    else
    {
        logger->Info("TryPulloverOnFoot");
        TryPulloverOnFoot();
    }

    blockInput = !blockInput;
}

void Pullover::TryPulloverFromVehicle()
{
    TryPulloverClosestVehicle();
}

void Pullover::TryPulloverOnFoot()
{
    logger->Info("try pullover on foot");

    if (aimingPed == NO_PED_FOUND)
    {
        //menuDebug->AddLine("~r~no ped found");

        TryPulloverClosestVehicle();
        return;
    }

    auto ped = Peds::GetPed(aimingPed);

    PulloverPed(ped);

    aimingPed = NO_PED_FOUND;
}

void Pullover::TryPulloverClosestVehicle()
{
    logger->Info("TryPulloverClosestVehicle");

    int playerActor = GET_PLAYER_ACTOR(0);

    float range = 4.0f;
    if (IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    logger->Info("aqui 1");

    auto playerPos = CVector(x, y, z);

    logger->Info("aqui 2");

    Vehicle* closestCar = Vehicles::GetClosestVehicleNotPlayer(playerPos, playerPos, 5.0f);

    logger->Info("2");

    if (closestCar == NULL)
    {
        if (IS_CHAR_IN_ANY_CAR(playerActor))
        {
            //
            BottomMessage::SetMessage(GetTranslatedText("pullover_from_vehicle_fail"), 3000);
        }
        else
        {
            BottomMessage::SetMessage(GetTranslatedText("pullover_on_foot_fail"), 3000);
        }

        return;
    }

    logger->Info("3");

    PulloverVehicle(closestCar);
}

void Pullover::PulloverPed(Ped* ped)
{
    if (ped->flags.isInconcious)
    {
        //menuDebug->AddLine("~g~pull over ped incounsious");

        ped->flags.showWidget = true;
        return;
    }

    AudioCollection::PlayAsVoice(audioPulloverPed);

    //menuDebug->AddLine("~g~pull over ped");

    if (ped->flags.willSurrender == false && ped->flags.willKillCops == true)
    {
        ped->ShowBlip(COLOR_CRIMINAL);
        Criminals::AddCriminal(ped);

        ModelLoader::AddModelToLoad(346);
        ModelLoader::LoadAll([ped]() { GIVE_ACTOR_WEAPON(ped->ref, 22, 500); });

        KILL_ACTOR(ped->ref, GetPlayerActor());

        return;
    }

    ped->flags.hasSurrended = true;
    ped->flags.showWidget = true;
    ped->SetCanDoHandsup();
    ped->ShowBlip(COLOR_CRIMINAL);
    Criminals::AddCriminal(ped);
}

void Pullover::FreePed(Ped* ped)
{
    //menuDebug->AddLine("~g~free ped");

    AudioCollection::PlayAsVoice(audioFreePed);

    ped->flags.showWidget = false;
    ped->flags.hasSurrended = false;
    ped->ClearAnim();
    ped->HideBlip();
    Criminals::RemoveCriminal(ped);
}

void Pullover::PulloverVehicle(Vehicle* vehicle)
{
    if (vehicle->HasDriver())
    {
        BottomMessage::SetMessage(GetTranslatedText("officer_pullover_vehicle"), 3000);

        AudioCollection::PlayAsVoice(audioPulloverCar);
    }

    if (vehicle->HasDriver())
    {
        auto driver = Peds::GetPed(vehicle->GetCurrentDriver());

        if (driver->flags.willSurrender == false)
        {
            BottomMessage::SetMessage(GetTranslatedText("suspect_ran_away"), 3000);

            Chase::StartChaseWithVehicle(vehicle);

            return;
        }
    }

    vehicle->ShowBlip(COLOR_CRIMINAL);
    vehicle->flags.showWidget = true;

    if (vehicle->HasDriver())
    {
        WAIT(1000, [vehicle]() { CAR_TURN_OFF_ENGINE(vehicle->ref); });

        WAIT(2500, []() { BottomMessage::SetMessage(GetTranslatedText("pullover_get_close_to_vehicle"), 3000); });
    }
    else
    {
        BottomMessage::SetMessage(GetTranslatedText("pullover_get_close_to_vehicle"), 3000);
    }
}

void Pullover::FreeVehicle(Vehicle* vehicle)
{
    vehicle->HideBlip();
    vehicle->flags.showWidget = false;

    vehicle->ValidateOwners();
    vehicle->MakeOwnersEnter();

    auto owners = vehicle->GetOwners();
    for (auto pedRef : owners)
    {
        auto ped = Peds::GetPed(pedRef);

        if (ped)
        {
            Criminals::RemoveCriminal(ped);
            ped->HideBlip();
            ped->flags.hasSurrended = false;
            ped->flags.showWidget = false;
        }
    }

    AudioCollection::PlayAsVoice(audioFreePed);

    CleoFunctions::AddWaitForFunction(
        "passengers_enter_vehicle_to_free",
        [vehicle]()
        {
            if (!Vehicles::IsValid(vehicle)) return true;

            auto ownersCount = vehicle->GetOwners().size();
            auto ocuppantsCount = vehicle->GetCurrentOccupants().size();

            if (ocuppantsCount >= ownersCount) return true;

            return false;
        },
        [vehicle]()
        {
            if (!Vehicles::IsValid(vehicle)) return;

            auto driverRef = vehicle->GetCurrentDriver();

            if (!ACTOR_DEFINED(driverRef)) return;

            auto driver = Peds::GetPed(driverRef);
            driver->StartDrivingRandomly();
        });
}

int Pullover::FindAimingPed()
{
    for (int ped = 1; ped < 35584; ped++)
    {
        if (PLAYER_AIMING_AT_ACTOR(0, ped))
        {
            aimingPed = ped;
            return ped;
        }
    }
    return NO_PED_FOUND;
}

void Pullover::OpenPedMenu(Ped* ped)
{
    g_blockInteractions = true;

    auto window = CreatePM_Window(GetTranslatedText("window_pullover_title"), "Ped");

    window->onClose->Add([]() { g_blockInteractions = false; });

    if (ped->flags.isInconcious)
    {
        {
            auto button = window->AddButton(GetTranslatedText("reanimate_ped"),
                [window, ped]()
                {
                    window->Close();

                    ped->Reanimate();
                });
        }

        {
            auto button = window->AddButton("Teleport to hospital",
                [window, ped]()
                {
                    window->Close();

                    Criminals::RemoveCriminal(ped);

                    ped->QueueDestroy();
                });
        }
    }

    if (ped->flags.isInconcious == false)
    {
        auto button = window->AddButton(GetTranslatedText("ask_for_rg"),
            [window, ped]()
            {
                window->Close();

                BottomMessage::SetMessage(GetTranslatedText("dialog_ask_for_rg"), 3000);

                AudioCollection::PlayAsVoice(audioAskRG,
                    [ped]()
                    {
                        ped->flags.shownRG = true;

                        RGWindow::CreateRG(ped);
                        //DocsWindow::ShowRG(ped);
                    });
            });
    }

    if (ped->flags.isInconcious == false)
    {
        auto button = window->AddButton(GetTranslatedText("ask_for_cnh"),
            [window, ped]()
            {
                window->Close();
                g_blockInteractions = true;

                BottomMessage::SetMessage(GetTranslatedText("dialog_ask_for_cnh"), 3000);

                AudioCollection::PlayAsVoice(audioAskCNH,
                    [ped]()
                    {
                        g_blockInteractions = false;
                        CNHWindow::CreateCNH(ped);
                        //DocsWindow::ShowCNH(ped);
                    });
            });
    }

    if (ped->flags.isInconcious == false)
    {
        if (ped->vehicleOwned > 0)
        {
            auto vehicle = Vehicles::GetVehicle(ped->vehicleOwned);

            auto button = window->AddButton(GetTranslatedText("ask_for_crlv"),
                [window, ped, vehicle]()
                {
                    window->Close();

                    if (vehicle->originalDoc.isStolen) { BottomMessage::SetMessage("Eu nao tenho o documento do veiculo..", 3000); }
                    else
                    {
                        BottomMessage::SetMessage(GetTranslatedText("dialog_ask_for_crlv"), 3000);

                        g_blockInteractions = true;

                        AudioCollection::PlayAsVoice(audioAskCRLV,
                            [ped, vehicle]()
                            {
                                g_blockInteractions = false;
                                DocsWindow::ShowCRLV(ped, vehicle);
                            });
                    }
                });
        }
    }

    if (ped->flags.isInconcious == false)
    {
        auto button = window->AddButton(GetTranslatedText("frisk_ped"),
            [window, ped]()
            {
                window->Close();

                ped->flags.showBackCheckpoint = true;
                ped->flags.canShowFriskMenu = true;

                //FriskWindow::OpenForPed(ped);
            });
    }

    if (ped->flags.isInconcious == false)
    {
        auto button = window->AddButton(GetTranslatedText("escort_ped"),
            [window, ped]()
            {
                window->Close();

                Escort::OpenEscortWindow(ped);
            });
    }

    if (ped->flags.isInconcious == false)
    {
        auto button = window->AddButton("Teleport to prision",
            [window, ped]()
            {
                window->Close();

                Criminals::RemoveCriminal(ped);

                ped->QueueDestroy();
            });
    }

    if (ped->flags.isInconcious == false)
    {
        {
            auto button = window->AddButton(GetTranslatedText("hands_on_head"),
                [window, ped]()
                {
                    window->Close();

                    AudioCollection::PlayAsVoice(audioPutHandsHead);

                    ped->SetAnim("handscower", "PED");
                });
        }

        {
            auto button = window->AddButton(GetTranslatedText("hands_behind"),
                [window, ped]()
                {
                    window->Close();

                    AudioCollection::PlayAsVoice(audioPutHandsBehind);

                    ped->SetAnim("bomber", "PED");
                });
        }
    }

    if (ped->flags.isInconcious == false)
    {
        if (ped->vehicleOwned > 0)
        {
            auto vehicle = Vehicles::GetVehicle(ped->vehicleOwned);

            auto button = window->AddButton("~r~" + GetTranslatedText("free_vehicle"),
                [window, vehicle]()
                {
                    window->Close();

                    if (!Vehicles::IsValid(vehicle))
                    {
                        BottomMessage::SetMessage("~r~An error ocurred", 2000);
                        return;
                    }

                    FreeVehicle(vehicle);
                });
        }
        else
        {
            auto button = window->AddButton("~r~" + GetTranslatedText("free_ped"),
                [window, ped]()
                {
                    window->Close();

                    FreePed(ped);
                });
        }
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void Pullover::OpenVehicleMenu(Vehicle* vehicle)
{
    auto window = CreatePM_Window(GetTranslatedText("window_pullover_title"), "Vehicle");

    bool vehicleIsEmpty = vehicle->GetCurrentOccupants().size() == 0;

    if (vehicleIsEmpty == false)
    {
        auto button = window->AddButton(GetTranslatedText("ask_leave_vehicle"),
            [window, vehicle]()
            {
                window->Close();

                AudioCollection::PlayAsVoice(audioExitVehicleHandsUp,
                    [vehicle]()
                    {
                        auto ocuppants = vehicle->GetCurrentOccupants();

                        vehicle->SetOwners();
                        vehicle->MakeOccupantsLeave();

                        for (auto pedRef : ocuppants)
                        {
                            auto ped = Peds::GetPed(pedRef);

                            Criminals::AddCriminal(ped);
                            ped->ShowBlip(COLOR_CRIMINAL);
                            ped->flags.hasSurrended = true;
                            ped->flags.showWidget = true;
                            ped->SetCanDoHandsup();
                        }
                    });
            });
    }

    if (vehicleIsEmpty == false)
    {
        auto button = window->AddButton(GetTranslatedText("ask_move_to_the_right"),
            [window, vehicle]()
            {
                window->Close();

                AskVehicleToMoveToTheRight(vehicle);
            });
    }

    if (vehicleIsEmpty)
    {
        auto button = window->AddButton(GetTranslatedText("call_tow_truck"),
            [window, vehicle]()
            {
                window->Close();

                auto owners = vehicle->GetOwners();

                for (auto pedRef : owners)
                {
                    auto ped = Peds::GetPed(pedRef);
                    ped->vehicleOwned = -1;
                    ped->UpdateSeatPosition();
                }

                vehicle->flags.showWidget = false;

                auto audio = audioRequestTowTruck->GetRandomAudio();

                RadioSounds::PlayAudioNowDontAttach(audio);

                WaitForAudioFinish(audio, [vehicle]() { CallTowTruck(vehicle); });
            });
    }

    {
        auto button = window->AddButton(GetTranslatedText("check_vehicle"),
            [window, vehicle]()
            {
                window->Close();

                DocsWindow::ShowVehicleVisualInfo(vehicle);
            });
    }

    if (vehicle->policeVehicleData != nullptr)
    {
        auto button = window->AddButton(GetTranslatedText("customize_trunk"),
            [window, vehicle]()
            {
                window->Close();

                vehicle->trunk->CreatePreviewPeds();

                WAIT(200, [vehicle]() { Trunk::OpenCustomizeMenu(vehicle->ref); });
            });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("free_vehicle"),
            [window, vehicle]()
            {
                window->Close();

                FreeVehicle(vehicle);
            });
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void Pullover::CallTowTruck(Vehicle* vehicle)
{
    logger->Info("Pullover: CallTowTruck");

    int towModelId = 578;
    int driverId = 50;

    ModelLoader::AddModelToLoad(towModelId);
    ModelLoader::AddModelToLoad(driverId);

    ModelLoader::LoadAll(
        [towModelId, driverId, vehicle]()
        {
            logger->Info("spawning tow truck...");

            auto playerPosition = GetPlayerPosition();
            auto spawnPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y + 100, playerPosition.z);

            auto towRef = CREATE_CAR_AT(towModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
            auto towTruck = Vehicles::RegisterVehicle(towRef);

            towTruck->ShowBlip(COLOR_YELLOW);

            auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(towRef, PedType::CivFemale, driverId);
            Peds::RegisterPed(driverRef);

            auto targetPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y, playerPosition.z);

            ScriptTask* taskDrive = new ScriptTask("drive");

            taskDrive->onBegin = [towRef, targetPosition]()
            {
                SET_CAR_MAX_SPEED(towRef, 20.0f);
                SET_CAR_TRAFFIC_BEHAVIOUR(towRef, DrivingMode::AvoidCars);
                CAR_DRIVE_TO(towRef, targetPosition.x, targetPosition.y, targetPosition.z);
            };
            taskDrive->onExecute = [towTruck, targetPosition]()
            {
                if (!Vehicles::IsValid(towTruck)) return SCRIPT_CANCEL;

                if (RadioWindow::m_cancelServices) return SCRIPT_CANCEL;

                auto distance = DistanceFromVehicle(towTruck->ref, targetPosition);

                if (distance < 10.0f) return SCRIPT_SUCCESS;

                return SCRIPT_KEEP_GOING;
            };
            taskDrive->onCancel = [towTruck]()
            {
                TopMessage::ClearMessage();

                if (!Vehicles::IsValid(towTruck)) return;

                BottomMessage::SetMessage("Guincho cancelado!", 3000);

                if (Vehicles::IsValid(towTruck)) { towTruck->QueueDestroy(true); }
            };
            taskDrive->onComplete = [vehicle, towTruck]()
            {
                TopMessage::ClearMessage();

                //

                CVector offset = CVector(0, -0.5, 0.6);

                if (Vehicles::IsValid(vehicle)) { ATTACH_CAR_TO_CAR(vehicle->ref, towTruck->ref, offset.x, offset.y, offset.z, 0, 0, 0); }

                vehicle->HideBlip();

                //

                ScriptTask* taskLeave = new ScriptTask("task_leave");
                taskLeave->onBegin = [towTruck]() { ScriptTask::MakeVehicleLeave(towTruck->ref); };
                taskLeave->onExecute = [towTruck]()
                {
                    if (!Vehicles::IsValid(towTruck)) return SCRIPT_CANCEL;

                    auto towPosition = GetCarPosition(towTruck->ref);
                    auto distance = DistanceFromPed(GetPlayerActor(), towPosition);

                    //menuDebug->AddLine("leave, " + std::to_string(distance));

                    if (distance > 120.0f) return SCRIPT_SUCCESS;

                    return SCRIPT_KEEP_GOING;
                };
                taskLeave->onComplete = [vehicle, towTruck]()
                {
                    //on complete

                    if (Vehicles::IsValid(vehicle)) { vehicle->QueueDestroy(true); }

                    if (Vehicles::IsValid(towTruck)) { towTruck->QueueDestroy(true); }
                };
                taskLeave->Start();
            };
            taskDrive->Start();

            TopMessage::SetMessage(GetTranslatedText("wait_for_tow_truck"));
        });
}

void Pullover::AskVehicleToMoveToTheRight(Vehicle* vehicle)
{
    AudioCollection::PlayAsVoice(audioMoveVehicleToRight);

    auto stopPosition = GetCarPositionWithOffset(vehicle->ref, CVector(5.0f, 7.0f, 0));

    auto sphere = CREATE_SPHERE(stopPosition.x, stopPosition.y, stopPosition.z, 1.0f);

    SET_CAR_ENGINE_OPERATION(vehicle->ref, true);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::StopForCars);
    SET_CAR_MAX_SPEED(vehicle->ref, 30.0f);
    CAR_DRIVE_TO(vehicle->ref, stopPosition.x, stopPosition.y, stopPosition.z);

    WAIT(2000, [sphere]() { DESTROY_SPHERE(sphere); });
}