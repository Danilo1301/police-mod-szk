#pragma once

#include "../pch.h"

class RadioSounds
{
  public:
    static void Initialize();
    static void Update();

    static void PlayNext();
    static void AttachAudio();
    static void PlayAudioNow(IAudio *audio);
    static void PlayAudioNowDontAttach(IAudio *audio);
};