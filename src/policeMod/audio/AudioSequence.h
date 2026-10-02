#pragma once

#include "../../pch.h"

#include "AudioVariationGroup.h"

class AudioSequence
{
  private:
    bool finished = false;
    int currentIndex = 0;

  public:
    AudioVariationGroup *group;

    AudioSequence();
    ~AudioSequence();

    void Play();
    void PlayNext();

    bool Finished();

    static void ProcessAudios();
};