#pragma once

#include "mod/iaml.h"
#include "mod/logger.h"

inline uintptr_t pGTASA;
inline void *hGTASA;

inline int (*GetPedRef)(void *);
inline int (*GetVehicleRef)(void *);
inline void *(*GetPedFromRef)(int);
inline void *(*GetVehicleFromRef)(int);

inline void DoHooks()
{
    logger->Info("Hooking...");

    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");

    SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));

    logger->Info("Hook OK!");
}