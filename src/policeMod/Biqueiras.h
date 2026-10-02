#pragma once

#include "../pch.h"

#include "Location.h"

class Biqueiras
{
  public:
    static void Initialize();

    static Location GetRandomCloseBiqueira(CVector point);
};