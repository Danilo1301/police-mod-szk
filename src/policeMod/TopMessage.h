#pragma once

#include "../pch.h"

class TopMessage
{
  public:
    static void Initialize();

    static void Update();

    static void SetMessage(const std::string &message);
    static void ClearMessage();
};