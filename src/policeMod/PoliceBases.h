#pragma once

#include "../pch.h"

#include "PoliceBase.h"

class PoliceBases
{
  private:
    static std::vector<PoliceBase *> Bases;

  public:
    static void Initialize();
    static void LoadBases();
    static void Update();
    static void OnPostDrawRadar();
    static void OnDraw();
};