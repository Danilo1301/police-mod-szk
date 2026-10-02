#pragma once

#include "../../pch.h"
#include "../Ped.h"
#include "AIPed.h"


class AIMedic : public AIPed
{
  public:
    CVector targetPosition = CVector();

    int targetPedRef = -1;
    std::vector<int> targetPeds;

    bool isDrivingAway = false;
    bool leftOnce = false;

    ~AIMedic() override;

    void Start() override;
    void Update() override;

    void FindTarget();

    void DoAction();

    void AproachPed(Ped *ped);
    void RessurectPed(Ped *ped);

    static bool AllMedicsAreDone(int vehicleRef);
};