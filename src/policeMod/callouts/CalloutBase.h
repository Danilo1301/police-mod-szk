#pragma once

#include "../audio/AudioVariationGroup.h"

class CalloutBase {
public:
    virtual ~CalloutBase() = default;

    virtual std::string GetBroadcastMessage() = 0;
    virtual AudioVariationGroup* GetBroadcastAudio() = 0;

    virtual void OnAccept() = 0;
};