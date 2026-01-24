#include <mod/amlmod.h>
#include <mod/config.h>

MYMODCFG(com.daniloszk.policemod, PoliceMod, 1.0.1, DaniloSZK)

#include "pch.h"

#include "PoliceMod.h"

cleo_ifs_t* cleo = NULL;
ISAUtils* sautils = NULL;
IMenuSZK* menuSZK = NULL;

ModData* modData = new ModData("policeModSZK");
Logger* localLogger = new Logger("policeModSZK");

extern "C" void OnModPreLoad()
{
    fileLog->Clear();
    fileLog->Log("Log initialized");
    fileLog->Log("OnModPreLoad");

    modData->LoadSettings();
    
    policeMod->OnModPreLoad();
}

extern "C" void OnModLoad()
{
    fileLog->Log("OnModLoad");

    loadInterface(&cleo, "CLEO", true);
    if(!cleo) return;

    loadInterface(&menuSZK, "MenuSZK");
    if(!menuSZK) return;

    loadInterface(&sautils, "SAUtils");
    if(!sautils) return;

    fileLog->Log("Finding addresses...");

    DoHooks();

    fileLog->Log("Initializing policeMod...");

    policeMod->OnModLoad();

    fileLog->Log("Mod loaded!");
}