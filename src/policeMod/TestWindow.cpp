#include "TestWindow.h"

#include "opcodeCaller/CleoFunctions.h"
#include "ModelLoader.h"
#include "Peds.h"
#include "BottomMessage.h"
#include "Chase.h"
#include "Callouts.h"
#include "Vehicles.h"
#include "Partners.h"
#include "callouts/CalloutRegistry.h"

void TestWindow::OpenWindow()
{
    static int calloutIndex = 0;

    auto window = CreatePM_Window("Mod Policia - tests", "Tests");

    {
        auto button = window->AddButton("Equip",
            [window]()
            {
                window->Close();

                TestEquip();
            });
    }

    {
        auto item = window->AddIntOptions("Callout: ID", &calloutIndex, 0, CalloutRegistry::m_callouts.size() - 1, 1);
    }

    {
        auto button = window->AddButton("Callout: Spawn",
            [window]()
            {
                window->Close();

                auto callouts = CalloutRegistry::m_callouts;
                auto ptr = callouts[calloutIndex];
                ;

                Callouts::BroadcastCallout(ptr);
            });
    }

    {
        auto button = window->AddButton("Freeze criminals",
            [window]()
            {
                window->Close();

                auto vehicles = Chase::vehiclesInChase;

                for (auto vehicle : vehicles) { SET_CAR_MAX_SPEED(vehicle->ref, 0); }

                BottomMessage::SetMessage("Velocity limited to 0", 2000);
            });
    }

    {
        auto button = window->AddButton("Spawn dangerous ped",
            [window]()
            {
                window->Close();

                BottomMessage::SetMessage("Spawnando pedestre que reage a abordagem", 3000);

                ModelLoader::AddModelToLoad(80);
                ModelLoader::LoadAll(
                    []()
                    {
                        auto playerPosition = GetPlayerPosition();
                        auto pedPath = STORE_PED_PATH_COORDS_CLOSEST_TO(playerPosition.x, playerPosition.y, playerPosition.z);

                        auto pedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, 80, pedPath.x, pedPath.y, pedPath.z);
                        auto ped = Peds::RegisterPed(pedRef);
                        ped->flags.willSurrender = false;

                        ped->ShowBlip(CRGBA(0, 255, 255));
                    });
            });
    }

    {
        auto button = window->AddButton("Accept callout",
            [window]()
            {
                window->Close();

                Callouts::AcceptCallout();
            });
    }

    {
        auto button = window->AddButton("Partner",
            [window]()
            {
                window->Close();

                Partners::CreateSpawnPartnerMenu();
            });
    }

    {
        auto button = window->AddButton("Create chase car",
            [window]()
            {
                window->Close();

                ModelLoader::AddModelToLoad(404);
                ModelLoader::AddModelToLoad(15);
                ModelLoader::LoadAll(
                    []()
                    {
                        auto playerPosition = GetPlayerPosition();
                        auto spawnPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y, playerPosition.z);

                        auto carRef = CREATE_CAR_AT(404, spawnPosition.x, spawnPosition.y, spawnPosition.z);
                        auto car = Vehicles::RegisterVehicle(carRef);

                        auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carRef, PedType::Special, 15);
                        auto driver = Peds::RegisterPed(driverRef);

                        auto passengerRef = CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(carRef, PedType::Special, 15, 0);
                        auto passenger = Peds::RegisterPed(passengerRef);

                        car->originalDoc.isStolen = true;
                        car->TryInitializePedsInside();
                        car->ShowBlip(CRGBA(0, 255, 255));

                        driver->flags.willSurrender = false;
                        passenger->flags.willSurrender = false;
                    });
            });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void TestWindow::TestEquip()
{
    SET_MAX_WANTED_LEVEL_TO(0);
    SET_PLAYER_WANTED_LEVEL(0, 0);

    int playerActor = GET_PLAYER_ACTOR(0);

    if (!PedHasWeaponId(playerActor, 10)) { GIVE_ACTOR_WEAPON(playerActor, 10, 1); }

    ModelLoader::AddModelToLoad(356);
    ModelLoader::AddModelToLoad(346);
    ModelLoader::AddModelToLoad(280);
    ModelLoader::LoadAll(
        [playerActor]()
        {
            GIVE_ACTOR_WEAPON(playerActor, 31, 800);
            GIVE_ACTOR_WEAPON(playerActor, 22, 200);
            CHANGE_PLAYER_MODEL_TO(0, 280);
        });

    BottomMessage::SetMessage("Mod iniciado", 3000);

    //if (DEEP_LOG_ENABLED) { BottomMessage::SetMessage("Mod iniciado. ~r~DEEP LOG ~w~ativado", 3000); }
}