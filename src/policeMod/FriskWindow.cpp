#include "FriskWindow.h"

void FriskWindow::OpenForPed(Ped* ped)
{
    g_blockInteractions = true;

    auto window = CreatePM_Window(GetTranslatedText("window_frisk"), "Frisk");

    window->onClose->Add([]() { g_blockInteractions = false; });

    {
        auto itemCount = ped->inventory.GetItems().size();

        if (itemCount == 0) { window->AddItem("~r~Nothing here..."); }
    }

    for (const auto& item : ped->inventory.GetItems())
    {
        auto def = item.GetDefinition();
        int amount = item.GetAmount();

        std::string text = FormatAmount(*def, amount);

        auto button = window->AddButton(text, []() {});
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}
