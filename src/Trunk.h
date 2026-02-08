#pragma once

#include "pch.h"

class Trunk {
private:
    int vehicleRef;
    std::vector<int> pedsInside;
public:
    Trunk(int vehicleRef);

    void AddPedToTrunk(int pedRef);
    void RemoveAllPeds();
    void RemovePed(int pedRef);

    bool HasPedsInside() { return pedsInside.size() > 0; }
    std::vector<int> GetPedsInTrunk() { return pedsInside; }

    void CreatePreviewPeds();
    void UpdatePreviewPeds();

    void CheckForNulls();

    static void OpenCustomizeMenu(int vehicleRef);
};