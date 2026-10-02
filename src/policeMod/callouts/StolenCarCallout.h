#pragma once

#include "../../pch.h"

#include "CalloutBase.h"

class StolenCarCallout : public CalloutBase {
public:
    std::string GetBroadcastMessage() override;
    AudioVariationGroup* GetBroadcastAudio() override;

    void OnAccept() override;
};