#include "AudioCollection.h"

#include "../opcodeCaller/CleoFunctions.h"
#include "mod/logger.h"

std::map<std::string, AudioVariationGroup*> g_audioGroups;

IAudio* g_currentVoiceAudio = nullptr;

void AudioCollection::Initialize()
{
    const bool IN_3D = true;

    {
        auto group = audioPulloverCar = CreateGroup("pull_over_vehicle");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/pull_over_vehicle.wav"));
    }

    {
        auto group = audioPulloverPed = CreateGroup("pull_over_ped");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/pull_over_ped.wav"));
    }

    {
        auto group = audioFreePed = CreateGroup("free_ped");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/all_clear.wav"));
    }

    {
        auto group = audioAskRG = CreateGroup("ask_rg");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/ask_rg.wav"));
    }

    {
        auto group = audioAskCNH = CreateGroup("ask_cnh");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/ask_cnh.wav"));
    }

    {
        auto group = audioAskCRLV = CreateGroup("ask_crlv");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/ask_crlv.wav"));
    }

    {
        auto group = audioExitVehicleHandsUp = CreateGroup("exit_vehicle_hands_up");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/exit_vehicle_hands_up.wav"));
    }

    {
        auto group = audioPlateCheck = CreateGroup("plate_check");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/license_plate_check.wav"));
    }

    {
        auto group = audioRequestBackup = CreateGroup("request_backup");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/request_backup.wav"));
    }

    {
        auto group = audioRequestAmbulance = CreateGroup("request_ambulance");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/request_ambulance.wav"));
    }

    {
        auto group = audioCalloutATM = CreateGroup("callout_atm");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/callouts/atm.wav"), IN_3D);
    }

    {
        auto group = audioCalloutStolenCar = CreateGroup("callout_stolen_car");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/callouts/stolen_car.wav"), IN_3D);
    }

    {
        auto group = audioCalloutDrugDealer = CreateGroup("drug_dealer");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/callouts/drug_dealer.wav"), IN_3D);
    }

    {
        auto group = audioRequestTowTruck = CreateGroup("request_tow_truck");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/request_tow_truck.wav"));
    }

    {
        auto group = audioMoveVehicleToRight = CreateGroup("ask_to_move_vehicle_to_his_right");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/ask_to_move_vehicle_to_his_right.wav"));
    }

    {
        auto group = audioPutHandsBehind = CreateGroup("put_hands_behind");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/put_hands_behind.wav"));
    }

    {
        auto group = audioPutHandsHead = CreateGroup("put_hands_on_head");
        group->FindAndLoadAudioVariants(modData->GetFileFromAssets("audios/officer/put_hands_on_head.wav"));
    }
}

AudioVariationGroup* AudioCollection::CreateGroup(std::string key)
{
    auto group = new AudioVariationGroup();

    g_audioGroups[key] = group;

    return group;
}

AudioVariationGroup* AudioCollection::GetGroup(std::string key)
{
    auto it = g_audioGroups.find(key);
    if (it != g_audioGroups.end()) return it->second;

    logger->Error("[AudioCollection] Group not find: %s", key.c_str());
    return nullptr;
}

void AudioCollection::PlayAsVoice(AudioVariationGroup* group)
{
    if (g_currentVoiceAudio != nullptr)
    {
        // g_currentVoiceAudio->Stop();
    }

    auto audio = group->PlayRandom();

    g_currentVoiceAudio = audio;
}

void AudioCollection::PlayAsVoice(AudioVariationGroup* group, std::function<void()> callback)
{
    PlayAsVoice(group);

    WaitForAudioFinish(g_currentVoiceAudio, [callback]() { callback(); });
}

void WaitForAudioFinish(IAudio* audio, std::function<void()> callback)
{
    CleoFunctions::AddWaitForFunction(
        "wait_audio",
        [audio]()
        {
            if (!audio->Finished()) return false;

            return true;
        },
        [callback]() { callback(); });
}