#include "DocumentWindow.h"
#include "src/globals.h"

DocumentWindow::DocumentWindow(std::string title, Ped* ped)
{
    this->ped = ped;

    auto windowPosition = g_defaultMenuPosition;

    window = CreatePM_Window(title, "Document", 900);

    auto self = this;

    window->onClose->Add(
        [self]()
        {
            auto fn = self->onClose;

            delete self;

            if (fn) fn();
        });
}

DocumentWindow::~DocumentWindow()
{
}

void DocumentWindow::MakeWindow()
{
    // dont make window here
}