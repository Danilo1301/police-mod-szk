#include "AudioSequence.h"
#include <algorithm>

std::vector<AudioSequence*> g_audioSequences;

AudioSequence::AudioSequence()
{
    group = new AudioVariationGroup();
    currentIndex = 0;
    finished = false;

    g_audioSequences.push_back(this);
}

AudioSequence::~AudioSequence()
{
    delete group;
}

void AudioSequence::Play()
{
    if (group->audios.empty())
        return;

    currentIndex = 0;
    group->audios[currentIndex]->Play();
}

void AudioSequence::PlayNext()
{
    currentIndex++;

    if (currentIndex >= group->audios.size())
    {
        finished = true; // marca para remoção
        return;
    }

    group->audios[currentIndex]->Play();
}

bool AudioSequence::Finished()
{
    return finished || group->audios.empty() || currentIndex >= group->audios.size();
}

void AudioSequence::ProcessAudios()
{
    for (auto it = g_audioSequences.begin(); it != g_audioSequences.end();)
    {
        AudioSequence* sequence = *it;

        if (sequence->Finished())
        {
            delete sequence;
            it = g_audioSequences.erase(it);
            continue;
        }

        int index = sequence->currentIndex;

        if (!sequence->group->audios.empty())
        {
            auto audio = sequence->group->audios[index];
            if (audio->Finished())
                sequence->PlayNext();
        }

        ++it;
    }
}