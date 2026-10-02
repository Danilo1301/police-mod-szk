#include "DocsWindow.h"

#include "BottomMessage.h"
#include "opcodeCaller/CleoFunctions.h"
#include "Vehicles.h"
#include "audio/AudioCollection.h"
#include "audio/AudioSequence.h"
#include "RadioSounds.h"

void DocsWindow::ShowRG(Ped* ped)
{
    auto window = CreatePM_Window(GetTranslatedText("window_rg"), "RG", 800);
    //window->SetCloseButtonVisible(false);

    {
        window->AddItem("- ID: " + std::to_string(ped->ref));
    }

    {
        window->AddItem("- RG: " + ped->rg);
        window->AddItem("- CPF: " + ped->cpf);
    }

    {
        auto button = window->AddButton(GetTranslatedText("check_rg"),
            [window, ped]()
            {
                window->Close();

                BottomMessage::SetMessage(GetTranslatedText("checking_rg"), 5000);

                WAIT(5000, [ped]() { ShowRGResults(ped); });
            });
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void DocsWindow::ShowRGResults(Ped* ped)
{
    auto window = CreatePM_Window(GetTranslatedText("window_rg_results"), "RG", 800);

    window->SetCloseButtonVisible(false);

    if (ped->flags.wantedByJustice) { window->AddItem(GetTranslatedText("suspect_with_warrant")); }
    else
    {
        window->AddItem(GetTranslatedText("suspect_with_no_warrant"));
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void DocsWindow::ShowVehicleVisualInfo(Vehicle* vehicle)
{
    auto window = CreatePM_Window(GetTranslatedText("window_vehicle"), "Vehicle");

    if (vehicle == nullptr) return;

    {
        window->AddItem("- Placa: " + vehicle->currentDoc.plate);
    }

    {
        window->AddItem("- Cor: ?");
    }

    {
        auto button = window->AddButton(GetTranslatedText("check_vehicle_by_plate"),
            [window, vehicle]()
            {
                window->Close();

                BottomMessage::SetMessage(GetTranslatedText("checking_plate"), 5000);

                auto audio = audioPlateCheck->GetRandomAudio();

                RadioSounds::PlayAudioNowDontAttach(audio);

                WaitForAudioFinish(audio, [vehicle]() { ShowVehicleResults(vehicle, true); });
            });
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void DocsWindow::ShowVehicleResults(Vehicle* vehicle, bool byPlate)
{
    auto window = CreatePM_Window(GetTranslatedText("window_vehicle_results"), "Vehicle", 800);

    auto doc = byPlate ? vehicle->currentDoc : vehicle->originalDoc;

    window->AddItem("- Placa: " + doc.plate);
    window->AddItem("- Chassi: " + doc.chassis);

    if (doc.isStolen) { window->AddItem("- " + GetTranslatedText("vehicle_stolen")); }
    else
    {
        window->AddItem("- " + GetTranslatedText("vehicle_not_stolen"));
    }

    if (doc.isDocumentExpired) { window->AddItem("- " + GetTranslatedText("vehicle_doc_expired")); }
    else
    {
        window->AddItem("- " + GetTranslatedText("vehicle_doc_ok"));
    }

    if (vehicle->flags.swappedPlate)
    {
        window->AddItem("- ~r~" + GetTranslatedText("vehicle_caracteristics_not_ok"));

        if (byPlate)
        {
            auto button = window->AddButton(GetTranslatedText("check_vehicle_by_chassis"),
                [window, vehicle]()
                {
                    window->Close();

                    BottomMessage::SetMessage(GetTranslatedText("checking_chassis"), 5000);

                    WAIT(5000, [vehicle]() { ShowVehicleResults(vehicle, false); });
                });
        }
    }
    else
    {
        window->AddItem("- " + GetTranslatedText("vehicle_caracteristics_ok"));
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void DocsWindow::ShowCRLV(Ped* ped, Vehicle* vehicle)
{
    auto window = CreatePM_Window(GetTranslatedText("window_vehicle_results"), "Vehicle");

    window->AddItem("Placa: " + vehicle->originalDoc.plate);
    window->AddItem("RENAVAM: " + vehicle->originalDoc.renavam);
    window->AddItem("Chassi: " + vehicle->originalDoc.chassis);
    window->AddItem("CPF: " + ped->cpf);

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void DocsWindow::ShowCNH(Ped* ped)
{
    auto window = CreatePM_Window(GetTranslatedText("window_cnh"), "CNH");

    if (ped->catHab.length() > 0)
    {
        window->AddItem(GetTranslatedText("cnh_category") + ped->catHab);

        // if(ped->flags.expiredDriversLicense)
        // {
        //     window->AddText(GetTranslatedText("vehicle_cnh_expired"));
        // } else {
        //     window->AddText(GetTranslatedText("vehicle_cnh_ok"));
        // }
    }
    else
    {
        window->AddItem(GetTranslatedText("with_no_cnh"));
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void DocsWindow::ShowChassisResult(Vehicle* vehicle)
{
    auto window = CreatePM_Window(GetTranslatedText("window_vehicle_results"), "Vehicle");

    {
        if (vehicle->flags.chassisErased) { window->AddItem("- Chassis: ~r~Suprimido"); }
        else
        {
            window->AddItem("- Chassis: " + vehicle->originalDoc.chassis);
        }
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}