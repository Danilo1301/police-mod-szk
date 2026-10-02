
#pragma once

#include "DocumentWindow.h"

class CNHWindow : public DocumentWindow {
public:

    CNHWindow(Ped* ped);
    ~CNHWindow();

    void MakeWindow() override;
    
    static CNHWindow* CreateCNH(Ped* ped) {
        auto window = new CNHWindow(ped);
        window->MakeWindow();
        return window;
    }
};