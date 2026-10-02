#pragma once

#include "../pch.h"

#include "Location.h"

class ATMSystem
{
  public:
    static void Initialize();
    static void CreateObjects();

    static Location *GetRandomATMLocation();
    static Location *GetATMLocation(CVector position);
};