#pragma once

#include "../../pch.h"

class AudioVariationGroup
{
  public:
    std::vector<IAudio *> audios;

    IAudio *GetRandomAudio();
    IAudio *PlayRandom();

    void LoadNewAudio(std::string src, bool in3d = false);
    void FindAndLoadAudioVariants(std::string src, bool in3d = false);
};