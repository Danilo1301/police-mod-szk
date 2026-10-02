#include "TopMessage.h"
#include "include/menu.h"

IContainer* t_container = nullptr;
std::string t_message = "";

void TopMessage::Initialize()
{
    auto container = menuSZK->GetMainContainer()->AddChild_I("topMessage");

    container->style.left = "50%";
    container->style.width = "100%";
    container->style.top = "150px";
    container->style.transformOrigin = CVector2D(0, -1);
    container->style.textHorizontalAlign = HorizontalAlign::Middle;
    container->style.textVerticalAlign = VerticalAlign::Middle;

    container->text = "Aqui vai ficar a mensagme";

    auto font = &container->fontStyle;
    font->size = 3.0f;
    font->align = GameFontAlignment::ALIGN_CENTER;

    t_container = container;
}

void TopMessage::Update()
{
    if (!t_container) return;

    if (t_message.empty())
    {
        t_container->text = "";
        return;
    }

    t_container->text = t_message;
}

void TopMessage::SetMessage(const std::string& message)
{
    t_message = message;
}

void TopMessage::ClearMessage()
{
    t_message = "";
}