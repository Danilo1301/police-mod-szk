#pragma once

#include "menuSZK/imenuSZK.h"

struct WorldWidget
{
    IWidget* widget;
    int attachToPed = -1;
    int attachToVehicle = -1;
};

void UpdateWorldWidgets();

void UpdateWorldWidget(WorldWidget* wWidget);