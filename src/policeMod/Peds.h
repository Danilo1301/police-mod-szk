#pragma once

#include "../pch.h"

#include "Ped.h"

class Peds
{
  private:
    static std::map<int, Ped *> peds;

  public:
    static void AddPed(int ref, void *ptr);
    static Ped *RegisterPed(int ref);
    static void RemovePed(int ref);
    static bool IsValid(Ped *ped);
    static Ped *GetPed(int ref);
    static std::map<int, Ped *> GetPedsMap();
    static void Update();
};