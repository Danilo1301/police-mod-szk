#pragma once

#include "../../pch.h"

// #include "menuSZK/IAudio.h"

#include "AudioVariationGroup.h"

inline AudioVariationGroup *audioPulloverCar = nullptr;
inline AudioVariationGroup *audioPulloverPed = nullptr;
inline AudioVariationGroup *audioFreePed = nullptr;
inline AudioVariationGroup *audioAskRG = nullptr;
inline AudioVariationGroup *audioAskCNH = nullptr;
inline AudioVariationGroup *audioAskCRLV = nullptr;
inline AudioVariationGroup *audioExitVehicleHandsUp = nullptr;
inline AudioVariationGroup *audioPlateCheck = nullptr;
inline AudioVariationGroup *audioRequestBackup = nullptr;
inline AudioVariationGroup *audioRequestAmbulance = nullptr;
inline AudioVariationGroup *audioCalloutATM = nullptr;
inline AudioVariationGroup *audioCalloutStolenCar = nullptr;
inline AudioVariationGroup *audioCalloutDrugDealer = nullptr;
inline AudioVariationGroup *audioRequestTowTruck = nullptr;
inline AudioVariationGroup *audioMoveVehicleToRight = nullptr;
inline AudioVariationGroup *audioPutHandsBehind = nullptr;
inline AudioVariationGroup *audioPutHandsHead = nullptr;

class AudioCollection
{
  public:
    static void Initialize();

    static AudioVariationGroup *CreateGroup(std::string key);
    static AudioVariationGroup *GetGroup(std::string key);

    static void PlayAsVoice(AudioVariationGroup *group);
    static void PlayAsVoice(AudioVariationGroup *group, std::function<void()> callback);
};

void WaitForAudioFinish(IAudio *audio, std::function<void()> callback);