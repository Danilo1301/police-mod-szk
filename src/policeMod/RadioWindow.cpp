#include "RadioWindow.h"

#include "BottomMessage.h"
#include "Chase.h"
#include "opcodeCaller/CleoFunctions.h"
#include "BackupUnits.h"
#include "Callouts.h"
#include "TestWindow.h"
#include "ModelLoader.h"
#include "RadioSounds.h"

IContainer* RadioWindow::MainContainer = nullptr;
IContainer* RadioWindow::ScreenContainer = nullptr;

bool RadioWindow::m_cancelServices = false;

std::map<std::string, RadioScreenGroup> RadioWindow::groups;

std::string RadioWindow::currentGroupId;

int RadioWindow::currentScreenIndex = 0;

bool g_isWaitingForRadio = false;

void RadioWindow::Initialize()
{
    auto container = menuSZK->GetMainContainer()->AddChild_I("radio");

    container->style.left = "600px";
    container->style.bottom = "-150px";
    container->style.width = "600px";
    container->style.height = "1000px";
    container->style.backgroundImage = modData->GetFileFromAssets("radio/radio.png");
    container->canClickThrough = true;

    MainContainer = container;

    {
        auto container = MainContainer->AddChild_I("up");

        container->style.left = "450px";
        container->style.top = "450px";
        container->style.width = "100px";
        container->style.height = "100px";
        container->style.backgroundImage = modData->GetFileFromAssets("radio/button_up.png");

        container->onClick->Add([]() { PrevScreen(); });
    }

    {
        auto container = MainContainer->AddChild_I("down");

        container->style.left = "450px";
        container->style.top = "570px";
        container->style.width = "100px";
        container->style.height = "100px";
        container->style.backgroundImage = modData->GetFileFromAssets("radio/button_down.png");

        container->onClick->Add([]() { NextScreen(); });
    }

    {
        auto container = MainContainer->AddChild_I("select");

        container->style.left = "20px";
        container->style.top = "530px";
        container->style.width = "100px";
        container->style.height = "100px";
        container->style.backgroundImage = modData->GetFileFromAssets("radio/button_select.png");

        container->onClick->Add([]() { Select(); });
    }

    {
        auto container = MainContainer->AddChild_I("close");

        container->style.left = "20px";
        container->style.top = "720px";
        container->style.width = "100px";
        container->style.height = "100px";
        container->style.backgroundImage = modData->GetFileFromAssets("radio/button_close.png");

        container->onClick->Add([]() { Back(); });
    }

    {
        auto container = MainContainer->AddChild_I("radio_image");

        container->style.left = "198px";
        container->style.top = "576px";
        container->style.width = "180px";
        container->style.height = "170px";

        ScreenContainer = container;
    }

    AddGroup("main", "");
    currentGroupId = "main";
    currentScreenIndex = 0;

    AddScreen("main", "send_qth", "send_qth.png");
    AddScreen("main", "call_helicopter", "call_helicopter.png");
    AddScreen("main", "callout_menu", "callout_menu.png");
    AddScreen("main", "cancel_chase", "cancel_chase.png");
    AddScreen("main", "cancel_services", "cancel_services.png");
    AddScreen("main", "call_backup", "call_backup.png");
    AddScreen("main", "call_medic", "call_medic.png");
    AddScreen("main", "toggle_car_options", "toggle_car_options.png");
    AddScreen("main", "test_menu", "test_menu.png");

    // AddGroup("test", "main");

    // AddScreen("test", "test1", "test.png");
    // AddScreen("test", "test2", "test.png");
    // AddScreen("test", "test3", "test.png");

    AddGroup("callout", "main");

    AddScreen("callout", "callout_accept", "callout_accept.png");

    UpdateScreenContainer();

    MainContainer->visible = false;
}

void RadioWindow::Toggle()
{
    bool visible = !MainContainer->visible;

    MainContainer->visible = visible;

    if (visible) { ToggleRadioAnim(true); }
    else
    {
        if (g_isWaitingForRadio) return;
        ToggleRadioAnim(false);
    }
}

void RadioWindow::OnSelect(std::string id)
{
    if (id == "send_qth")
    {
        ToggleRadioAnimOff(5000);
        Toggle();
        BackupUnits::SendQTH();
        return;
    }

    if (id == "call_helicopter")
    {
        ToggleRadioAnimOff(5000);
        Toggle();

        BottomMessage::SetMessage("Chamando apoio do aguia...", 3000);

        BackupUnits::SpawnHelicopterBackup();
        return;
    }

    if (id == "callout_menu")
    {
        SwitchToGroup("callout");
        return;
    }

    if (id == "cancel_chase")
    {
        Toggle();
        Chase::AbortChase();
        return;
    }

    if (id == "cancel_services")
    {
        m_cancelServices = true;
        Toggle();

        WAIT(1000, []() { m_cancelServices = false; });

        return;
    }

    if (id == "call_backup")
    {
        Toggle();
        BackupUnits::OpenSpawnBackupMenu();
        return;
    }

    if (id == "call_medic")
    {
        ToggleRadioAnimOff(5000);
        Toggle();

        BottomMessage::SetMessage("Chamando o resgate...", 3000);

        auto audio = audioRequestAmbulance->GetRandomAudio();

        RadioSounds::PlayAudioNowDontAttach(audio);

        WaitForAudioFinish(audio,
            []()
            {
                auto playerPosition = GetPlayerPosition();

                BackupUnits::SpawnMedicUnit(playerPosition);
            });

        return;
    }

    if (id == "test_menu")
    {
        Toggle();
        TestWindow::OpenWindow();
        return;
    }

    if (id == "callout_accept")
    {
        if (Callouts::HasCalloutToAccept())
        {
            Toggle();
            Callouts::AcceptCallout();
            return;
        }

        BottomMessage::SetMessage("~r~There are no active callouts", 3000);

        return;
    }

    if (id == "toggle_car_options")
    {
        Toggle();

        g_canShowCarWidgetAnyTime = !g_canShowCarWidgetAnyTime;
        if (g_canShowCarWidgetAnyTime) { BottomMessage::SetMessage("~g~Ativado ~w~botao de opcoes dos veiculos", 3000); }
        else
        {
            BottomMessage::SetMessage("~r~Desativado ~w~botao de opcoes dos veiculos", 3000);
        }
        return;
    }

    BottomMessage::SetMessage("~r~No action defined", 1000);
}

int g_radioObject = 0;

void RadioWindow::ToggleRadioAnim(bool state)
{
    if (state == false && g_isWaitingForRadio) { return; }

    if (g_radioObject != 0)
    {
        DESTROY_OBJECT(g_radioObject);
        g_radioObject = 0;
    }

    if (state)
    {
        int radioModelId = 321;

        ModelLoader::AddModelToLoad(radioModelId);
        ModelLoader::LoadAll(
            [radioModelId]()
            {
                g_radioObject = CREATE_OBJECT(radioModelId, 0, 0, 0);
                ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION(GetPlayerActor(), g_radioObject, 0, 0, 0, 6, 16, "PHONE_TALK", "PED", 1);
            });
    }
}

void RadioWindow::ToggleRadioAnimOff(int timeMs)
{
    g_isWaitingForRadio = true;

    WAIT(timeMs,
        []()
        {
            g_isWaitingForRadio = false;
            ToggleRadioAnim(false);
        });
}