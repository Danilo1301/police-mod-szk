#pragma once

#include "../pch.h"

class Checkpoint;

class PoliceBase
{
public:
    CVector basePosition = CVector(0, 0, 0);

    CVector leaveCriminalPosition = CVector(0, 0, 0);
    Checkpoint* leaveCriminalCheckpoint = nullptr;

    CVector getPartnerPosition = CVector(0, 0, 0);
    Checkpoint* getPartnerCheckpoint = nullptr;

    PoliceBase();

    void Init();
    void Update();
    void OnPostDrawRadar();
    void OnDraw();
};