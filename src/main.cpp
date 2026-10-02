#include "main.h"

#include "globals.h"
#include "mod/logger.h"

#include "utils/logStorage.h"

#include "hooks.h"
#include "logHelper.h"

#include "policeMod/PoliceMod.h"
#include <string>

MYMODCFG(com.daniloszk.policemodszk64, PoliceMod SZK, 1.1.0, DaniloSZK);

void _OnLoggerMessage(eLogPrio prio, const char* msg)
{
    if (!msg) return;

    if (menuSZK) { menuSZK->AddLogMessage("PoliceMod: " + std::string(msg)); }
}

ON_MOD_PRELOAD()
{
    logger->SetTag("PoliceMod-PSDK");
    logger->Info("Mod Preload");

    logger->SetMessageCB(_OnLoggerMessage);
}

ON_MOD_LOAD()
{
    logger->Info("Mod loading...");

    menuSZK = (IMenuSZK*)GetInterface("menuSZK_v2");

    if (menuSZK == nullptr)
    {
        logger->Error("MenuSZK interface was not found. Do you have it installed?");
        return;
    }

    modData->LoadSettings();

    //InitLogStorage();

    DoHooks();

    logger->Info("Initializing policeMod...");

    policeMod->OnModLoad();

    logger->Info("Mod loaded");
}

ON_GAME_CRASH()
{
}