#include "CNHWindow.h"
#include "BottomMessage.h"
#include "opcodeCaller/CleoFunctions.h"
#include "DocsWindow.h"

CNHWindow::CNHWindow(Ped* ped) : DocumentWindow("CNH", ped)
{
}

CNHWindow::~CNHWindow()
{
}

void CNHWindow::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem("item", 600);

        auto rgContainer = item->GetContainer()->AddChild_I("cnhContainer");
        rgContainer->style.width = "100%";
        rgContainer->style.height = "100%";
        rgContainer->style.backgroundImage = modData->GetFileFromAssets("cnh.png");

        auto createLabel = [rgContainer]() -> IContainer*
        {
            auto label = rgContainer->AddChild_I("label");
            label->text = "This is a text Label";

            auto font = &label->fontStyle;
            font->color = CRGBA(0, 0, 0);
            font->size = 1.6f;
            font->align = GameFontAlignment::ALIGN_LEFT;
            font->dropShadowPosition = 0;

            label->style.width = "400px";
            label->style.height = "50px";

            return label;
        };

        {
            // Nome

            auto label = createLabel();
            label->text = ped->name;
            label->style.left = "17%";
            label->style.top = "23%";
        }

        {
            // CPF

            auto label = createLabel();
            label->text = ped->cpf;
            label->style.left = "51%";
            label->style.top = "43.5%";
        }

        {
            // Data Nascimento

            auto label = createLabel();
            label->text = ped->birthDate;
            label->style.left = "51%";
            label->style.top = "55.5%";
        }

        {
            // Cat Hab

            auto label = createLabel();
            label->text = ped->catHab;
            label->style.left = "73%";
            label->style.bottom = "10.5%";
            label->fontStyle.color = CRGBA(255, 0, 0);
        }

        {
            // Validade

            auto label = createLabel();
            label->text = ped->cnhExpireDate;
            label->fontStyle.size = 1.4;
            label->style.left = "51%";
            label->style.bottom = "0%";
        }

        {
            // Registro

            auto label = createLabel();
            label->text = std::to_string(ped->ref);
            label->style.left = "17%";
            label->style.bottom = "0%";
        }
    }

    {
        auto self = this;

        auto button = window->AddButton("~r~" + GetTranslatedText("close"), [self, this]() { window->Close(); });
    }
}