#pragma once

#include "../pch.h"

class Partners {
public:
    static void CreateSpawnPartnerMenu();

    static void SpawnPartner(CVector position);
    static void DestroyPartners();
};