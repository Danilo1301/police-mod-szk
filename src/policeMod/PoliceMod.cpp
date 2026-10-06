#include "PoliceMod.h"

#include "ATMSystem.h"
#include "BackupUnits.h"
#include "Biqueiras.h"
#include "BottomMessage.h"
#include "Callouts.h"
#include "Chase.h"
#include "Checkpoint.h"
#include "Criminals.h"
#include "Escort.h"
#include "Names.h"
#include "Peds.h"
#include "PoliceBases.h"
#include "PoliceVehicles.h"
#include "Pullover.h"
#include "RadioSounds.h"
#include "RadioWindow.h"
#include "TestWindow.h"
#include "TopMessage.h"
#include "Vehicles.h"
#include "WorldWidgets.h"
#include "ai/AIController.h"
#include "audio/AudioSequence.h"
#include "inventory/InventoryItemManager.h"
#include "mod/logger.h"
#include "opcodeCaller/CleoFunctions.h"
#include "../logHelper.h"

bool hasFirstUpdated = false;

PoliceMod::PoliceMod()
{
}

void PoliceMod::OnModLoad()
{
    // menuSZK->RegisterLocalizationFolder(menuSZK->GetLocalizationsPath() +"/policeModSZK/");

    menuSZK->onPedAdded->Add(
        [](GameEntity e)
        {
            BEGIN_OPERATION(op_onPedAdded);

            Peds::AddPed(e.ref, e.ptr);

            END_OPERATION(op_onPedAdded);
        });

    menuSZK->onPedRemoved->Add(
        [](GameEntity e)
        {
            BEGIN_OPERATION(op_onPedRemoved);

            logger->Info("onPedRemoved()");

            Peds::RemovePed(e.ref);

            END_OPERATION(op_onPedRemoved);
        });

    menuSZK->onVehicleAdded->Add(
        [](GameEntity e)
        {
            BEGIN_OPERATION(op_onVehicleAdded);

            Vehicles::AddVehicle(e.ref, e.ptr);

            END_OPERATION(op_onVehicleAdded);
        });

    menuSZK->onVehicleRemoved->Add(
        [](GameEntity e)
        {
            BEGIN_OPERATION(op_onVehicleRemoved);

            logger->Info("onVehicleRemoved()");

            g_onVehicleDestroy.Emit(e.ref);

            logger->Info("Removing veh");

            Vehicles::RemoveVehicle(e.ref);

            END_OPERATION(op_onVehicleRemoved);
        });

    menuSZK->onScriptProcess->Add(
        [this](unsigned int dt)
        {
            BEGIN_OPERATION(op_onGameUpdate);

            OnGameUpdate(dt);

            END_OPERATION(op_onGameUpdate);
        });

    menuSZK->onPlayerReady->Add([this]() { OnPlayerReady(); });

    textureBlip = menuSZK->GetOrLoadTexture(GetModAssetPath("blip.png"));
    textureCircle = menuSZK->GetOrLoadTexture(GetModAssetPath("map/circle.png"));
    textureBigCircle = menuSZK->GetOrLoadTexture(GetModAssetPath("map/big_circle.png"));
    texturePoliceDP = menuSZK->GetOrLoadTexture(GetModAssetPath("map/police_dep.png"));

    menuSZK->onDrawBeforeMenu->Add(
        [](unsigned int deltaTime)
        {
            auto peds = Peds::GetPedsMap();
            for (auto pair : peds)
            {
                auto ped = pair.second;

                if (!Peds::IsValid(ped)) continue;

                if (ped->flags.showBlip == false) continue;

                if (ped->IsInAnyCar()) continue;

                auto position = ped->GetPosition();
                position.z += 1.8f;

                DrawTextureOnWorld(textureBlip, position, ped->flags.blipColor, CVector2D(100, 100));
            }

            auto vehicles = Vehicles::GetVehiclesMap();
            for (auto pair : vehicles)
            {
                auto vehicle = pair.second;

                if (!Vehicles::IsValid(vehicle)) continue;

                if (vehicle->flags.showBlip == false) continue;

                auto position = vehicle->GetPosition();
                position.z += 2.2f;

                DrawTextureOnWorld(textureBlip, position, vehicle->GetBlipColor(), CVector2D(100, 100));
            }

            PoliceBases::OnDraw();
        });

    menuSZK->GetMainContainer()->onPreUpdateTransform->Add([]() { UpdateWorldWidgets(); });

    menuSZK->onPostDrawRadar->Add(
        []()
        {
            auto peds = Peds::GetPedsMap();
            for (auto pair : peds)
            {
                auto ped = pair.second;

                if (!Peds::IsValid(ped)) continue;

                if (ped->flags.showBlip == false) continue;

                if (ped->IsInAnyCar()) continue;

                auto position = ped->GetPosition();

                auto radarPoint = WorldToRadarPoint(position);

                menuSZK->DrawTextureOnRadar(textureCircle, radarPoint, ped->flags.blipColor, 14.0f);
            }

            auto vehicles = Vehicles::GetVehiclesMap();
            for (auto pair : vehicles)
            {
                auto vehicle = pair.second;

                if (!Vehicles::IsValid(vehicle)) continue;

                if (vehicle->flags.showBlip == false) continue;

                auto position = vehicle->GetPosition();

                auto radarPoint = WorldToRadarPoint(position);

                menuSZK->DrawTextureOnRadar(textureCircle, radarPoint, vehicle->GetBlipColor(), 20.0f);
            }

            PoliceBases::OnPostDrawRadar();
            BackupUnits::OnPostDrawRadar();
        });
}

void PoliceMod::OnGameUpdate(unsigned int deltaTime)
{
    g_deltaTime = deltaTime;

    if (!g_playerReady) return;

    BEGIN_OPERATION(op_checkUnits);
    BackupUnits::CheckIfVehiclesAreValid();
    END_OPERATION(op_checkUnits);

    BEGIN_OPERATION(op_checkCriminals);
    Criminals::CheckIfCriminalsAreValid();
    END_OPERATION(op_checkCriminals);

    BEGIN_OPERATION(op_getPlayerPosition);
    auto ppos = GetPlayerPosition();
    g_playerPosition = new CVector(ppos);
    END_OPERATION(op_getPlayerPosition);

    BEGIN_OPERATION(op_getPlayerVehicle);
    auto vehicleUsing = GetVehiclePedIsUsing(GetPlayerActor());

    if (vehicleUsing > 0) g_lastPlayerVehicle = vehicleUsing;

    if (!CAR_DEFINED(g_lastPlayerVehicle)) g_lastPlayerVehicle = -1;

    END_OPERATION(op_getPlayerVehicle);

    BEGIN_OPERATION(op_firstUpdate);

    if (!hasFirstUpdated)
    {
        hasFirstUpdated = true;
        OnFirstUpdate();
    }

    END_OPERATION(op_firstUpdate);

    BEGIN_OPERATION(op_pedsUpdate);
    Peds::Update();
    END_OPERATION(op_pedsUpdate);

    BEGIN_OPERATION(op_vehiclesUpdate);
    Vehicles::Update();
    END_OPERATION(op_vehiclesUpdate);

    BEGIN_OPERATION(op_pulloverUpdate);
    Pullover::Update();
    END_OPERATION(op_pulloverUpdate);

    BEGIN_OPERATION(op_criminalsUpdate);
    Criminals::Update();
    END_OPERATION(op_criminalsUpdate);

    BEGIN_OPERATION(op_bottomMessageUpdate);
    BottomMessage::Update();
    END_OPERATION(op_bottomMessageUpdate);

    BEGIN_OPERATION(op_topMessageUpdate);
    TopMessage::Update();
    END_OPERATION(op_topMessageUpdate);

    BEGIN_OPERATION(op_chaseUpdate);
    Chase::Update();
    END_OPERATION(op_chaseUpdate);

    BEGIN_OPERATION(op_backupUnitsUpdate);
    BackupUnits::Update();
    END_OPERATION(op_backupUnitsUpdate);

    BEGIN_OPERATION(op_aiControllerUpdate);
    AIController::Update();
    END_OPERATION(op_aiControllerUpdate);

    BEGIN_OPERATION(op_escortUpdate);
    Escort::Update();
    END_OPERATION(op_escortUpdate);

    BEGIN_OPERATION(op_policeBasesUpdate);
    PoliceBases::Update();
    END_OPERATION(op_policeBasesUpdate);

    BEGIN_OPERATION(op_checkpointsUpdate);
    Checkpoints::Update();
    END_OPERATION(op_checkpointsUpdate);

    BEGIN_OPERATION(op_calloutsUpdate);
    Callouts::Update();
    END_OPERATION(op_calloutsUpdate);

    BEGIN_OPERATION(op_audioSequence);
    AudioSequence::ProcessAudios();
    END_OPERATION(op_audioSequence);

    BEGIN_OPERATION(op_radioSoundsUpdate);
    RadioSounds::Update();
    END_OPERATION(op_radioSoundsUpdate);

    BEGIN_OPERATION(op_cleoFunctionsUpdate);
    CleoFunctions::Update(deltaTime);
    END_OPERATION(op_cleoFunctionsUpdate);

    BEGIN_OPERATION(op_destroyPeds);

    for (auto pedRef : g_pedsToDestroy)
    {
        if (!ACTOR_DEFINED(pedRef)) continue;

        DESTROY_ACTOR(pedRef);
        Peds::RemovePed(pedRef);
    }

    g_pedsToDestroy.clear();

    END_OPERATION(op_destroyPeds);

    BEGIN_OPERATION(op_destroyVehicles);

    for (auto carRef : g_vehiclesToDestroy)
    {
        if (!CAR_DEFINED(carRef)) continue;

        DESTROY_CAR(carRef);
        Vehicles::RemoveVehicle(carRef);
    }

    g_vehiclesToDestroy.clear();

    END_OPERATION(op_destroyVehicles);
}

void PoliceMod::OnFirstUpdate()
{
    logger->Info("mod polica: first update");

    LOAD_ANIMATION("GANGS");
    LOAD_ANIMATION("POLICE");
    LOAD_ANIMATION("MEDIC");
    LOAD_ANIMATION("CRACK");

    logger->Info("-1");

    logger->Info("InventoryItemManager");

    InventoryItemManager::Initialize();

    logger->Info("PoliceVehicles");

    PoliceVehicles::Initialize();

    logger->Info("PoliceBases");

    PoliceBases::Initialize();

    logger->Info("2");

    Biqueiras::Initialize();
    RadioSounds::Initialize();
    AudioCollection::Initialize();
    BottomMessage::Initialize();

    logger->Info("3");

    TopMessage::Initialize();
    RadioWindow::Initialize();
    BackupUnits::Initialize();
    Names::Initialize();

    logger->Info("4");

    Callouts::Initialize();
    ATMSystem::Initialize();

    logger->Info("mod polica: first update passed");
}

void PoliceMod::OnPlayerReady()
{
    g_playerReady = true;

    auto widgetStartPosition = g_widgetsStartPosition;

    auto widget = menuSZK->CreateWidget(
        widgetStartPosition.x + 170 + 170, widgetStartPosition.y, 150, "", GetModAssetPath("widgets/widget_vest.png"));

    widget->onClick->Add(
        [this, widget]()
        {
            widget->Destroy();

            TestWindow::TestEquip();

            CreateWidgets();
        });
}

void PoliceMod::CreateWidgets()
{
    auto widgetStartPosition = g_widgetsStartPosition;

    {
        auto widget =
            menuSZK->CreateWidget(widgetStartPosition.x, widgetStartPosition.y, 150, "", GetModAssetPath("widgets/widget_pullover.png"));

        widget->onClick->Add([]() { Pullover::OnClickWidget(); });
    }

    {
        auto widget =
            menuSZK->CreateWidget(widgetStartPosition.x + 170, widgetStartPosition.y, 150, "", GetModAssetPath("widgets/widget_radio.png"));

        widget->onClick->Add(
            []()
            {
                if (Callouts::HasCalloutToAccept())
                {
                    Callouts::OpenAcceptMenu(
                        [](bool accepted)
                        {
                            if (!accepted) { RadioWindow::Toggle(); }
                        });
                }
                else
                {
                    RadioWindow::Toggle();
                }
            });
    }
}

PoliceMod* policeMod = new PoliceMod();