#include "RGWindow.h"
#include "BottomMessage.h"
#include "DocsWindow.h"
#include "menuSZK/imenuSZK.h"
#include "opcodeCaller/CleoFunctions.h"

RGWindow::RGWindow(Ped* ped) : DocumentWindow("RG", ped)
{
}

RGWindow::~RGWindow()
{
}

void RGWindow::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem("item", 600);

        auto rgContainer = item->GetContainer()->AddChild_I("rgContainer");
        rgContainer->style.width = "100%";
        rgContainer->style.height = "100%";
        rgContainer->style.backgroundImage = modData->GetFileFromAssets("rg.png");

        //item->container->AddChild(rgContainer);

        auto createRGLabel = [rgContainer]() -> IContainer*
        {
            auto label = rgContainer->AddChild_I("label");
            label->text = "This is a text Label";
            label->style.textHorizontalAlign = HorizontalAlign::Left;
            label->style.textVerticalAlign = VerticalAlign::Middle;
            label->style.width = "400px";
            label->style.height = "50px";

            auto font = &label->fontStyle;
            font->color = CRGBA(30, 30, 30);
            font->size = 1.6f;
            font->align = GameFontAlignment::ALIGN_LEFT;
            font->dropShadowPosition = 0;

            return label;
        };

        {
            // CPF
            auto label = createRGLabel();
            label->text = ped->cpf;
            label->style.left = "16%";
            label->style.top = "10.5%";
        }

        {
            // RG
            auto label = createRGLabel();
            label->text = ped->rg;
            label->style.left = "39%";
            label->style.top = "19.5%";
        }

        {
            // Nome
            auto label = createRGLabel();
            label->text = ped->name;
            label->style.left = "20%";
            label->style.top = "29.5%";
        }

        {
            // Data Nascimento
            auto label = createRGLabel();
            label->text = ped->birthDate;
            label->style.left = "34%";
            label->style.top = "50.5%";
        }

        {
            // Ped ID
            auto label = createRGLabel();
            label->fontStyle.align = GameFontAlignment::ALIGN_RIGHT;
            label->text = std::to_string(ped->ref);
            label->style.right = "12%";
            label->style.top = "15%";
            label->style.textHorizontalAlign = HorizontalAlign::Right;
        }
    }

    {
        auto thisPed = this->ped;

        auto button = window->AddButton("> Consultar",
            [this, thisPed]()
            {
                window->Close();

                BottomMessage::SetMessage(GetTranslatedText("checking_rg"), 5000);

                WAIT(5000, [thisPed]() { DocsWindow::ShowRGResults(thisPed); });
            });
    }

    {
        auto self = this;

        auto button = window->AddButton("~r~" + GetTranslatedText("close"),
            [this]()
            {
                window->Close();

                // auto fn = onClose;

                // delete self;

                // if (fn) fn();
            });
    }
}