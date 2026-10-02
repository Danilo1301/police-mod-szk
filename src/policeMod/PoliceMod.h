#pragma once

#include "../pch.h"

class PoliceMod
{
public:
    PoliceMod();

    void OnModLoad();
    void OnGameUpdate(unsigned int deltaTime);
    void OnFirstUpdate();
    void OnPlayerReady();
    void CreateWidgets();
};

extern PoliceMod* policeMod;