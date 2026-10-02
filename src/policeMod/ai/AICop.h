#pragma once

#include "../../pch.h"
#include "AIPed.h"

class AICop : public AIPed
{
  public:
    int findTargetTimer = 0;
    int driveToTimer = 0;
    int targetPed = -1;
    int prevTargetPed = -1;
    bool leavingScene = false;

    ~AICop() override;

    void Start() override;
    void Update() override;

    void FindTarget();

    void DoAction();
};