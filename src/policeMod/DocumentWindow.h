#pragma once

#include "../pch.h"

#include "Ped.h"

class DocumentWindow
{
protected:
    IWindow* window;
    IContainer* container;

public:
    std::function<void()> onClose;
    Ped* ped;

    DocumentWindow(std::string title, Ped* ped);
    ~DocumentWindow();

    virtual void MakeWindow();
};