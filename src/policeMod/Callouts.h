#pragma once

#include "../pch.h"
#include <functional>

class CalloutBase;

#define NO_CALLOUT -1
#define CALLOUT_ATM 0

inline int g_secondsBetweenCallouts = 70;

class Callouts
{
public:
    static void Initialize();
    static void Update();

    static void TryBroadcastCallout();
    static void BroadcastRandomCallout();
    static void BroadcastCallout(CalloutBase* callout);

    static void AcceptCallout();

    static bool HasCalloutToAccept();
    static bool IsBroacastingCallout();
    static bool IsInCallout();

    static void OpenAcceptMenu(std::function<void(bool)> callback);
};