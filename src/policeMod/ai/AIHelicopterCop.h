#pragma once

#include "../../pch.h"
#include "AIPed.h"

class AIHelicopterCop : public AIPed
{
  public:
    int timer_findTarget = 0;
    int targetCriminal = -1;

    bool isLeaving = false;

    ~AIHelicopterCop() override;

    void Start() override;
    void Update() override;

    void FindTarget();

    void DoAction();
};