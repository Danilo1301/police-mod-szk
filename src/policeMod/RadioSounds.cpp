#include "RadioSounds.h"

#include "aml-psdk/game_sa/entity/Placeable.h"
#include "opcodeCaller/CleoFunctions.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "Callouts.h"
#include "Skins.h"
#include "../globals.h"
#include "../hooks.h"

std::vector<IAudio*> g_radioAudios;
std::vector<int> g_playedIndices; // guarda os índices já tocados

IAudio* g_currentAudio = nullptr;
int g_currentIndex = -1;

int g_nextPlayDelay = 0;
const int RADIO_DELAY_MIN = 3000;
const int RADIO_DELAY_MAX = 7000;

void RadioSounds::Initialize() {
    std::string folder = modData->GetFileFromAssets("audios/radio/");
    g_radioAudios.clear();

    try {
        // percorre todos os arquivos da pasta
        for (const auto& entry : std::filesystem::directory_iterator(folder)) {
            if (!entry.is_regular_file()) continue;

            auto path = entry.path();

            // só carrega arquivos .wav
            if (path.extension() == ".wav") {
                auto audio = menuSZK->GetOrLoad3DAudio(path.string());
                g_radioAudios.push_back(audio);
            }
        }
    } catch (const std::exception& e) {}
}

void RadioSounds::Update() {
    auto playerActor = GetPlayerActor();
    auto skinModel = GET_ACTOR_MODEL(playerActor);

    if (!IsPoliceSkin(skinModel)) {
        Callouts::TryBroadcastCallout();
        return;
    }

    // Atualiza o delay entre as falas
    if (g_nextPlayDelay > 0) {
        bool countTime = false;

        if (g_currentAudio == nullptr) {
            countTime = true;
        } else {
            countTime = g_currentAudio->Finished();
        }

        if (countTime) {
            g_nextPlayDelay -= g_deltaTime; // ou o delta time do seu sistema
        }
        return;
    }

    if (g_currentAudio != nullptr) {
        if (g_currentAudio->Finished()) {
            g_currentAudio = nullptr;

            // Espera aleatoriamente de 2 a 5 segundos antes do próximo áudio
            g_nextPlayDelay = getRandomNumber(RADIO_DELAY_MIN, RADIO_DELAY_MAX);

            return;
        }
    }

    // Se não há áudio tocando, tenta iniciar uma transmissão
    if (g_currentAudio == nullptr) { Callouts::TryBroadcastCallout(); }

    // Se está transmitindo ou em um callout, não toca rádio
    if (Callouts::IsBroacastingCallout() || Callouts::IsInCallout()) return;

    // Se ainda não há áudio, toca o próximo
    if (!g_currentAudio) { PlayNext(); }
}

void RadioSounds::PlayNext() {
    if (g_radioAudios.empty()) return;

    // todos já foram tocados → reseta
    if ((int)g_playedIndices.size() == (int)g_radioAudios.size()) g_playedIndices.clear();

    // escolhe um índice que ainda não foi tocado
    int nextIndex;
    do {
        nextIndex = rand() % g_radioAudios.size();
    } while (std::find(g_playedIndices.begin(), g_playedIndices.end(), nextIndex) != g_playedIndices.end());

    g_playedIndices.push_back(nextIndex); // marca como já tocado

    g_currentIndex = nextIndex;
    g_currentAudio = g_radioAudios[g_currentIndex];

    AttachAudio();

    g_currentAudio->Play();
}

void RadioSounds::AttachAudio() {
    CPed* player = (CPed*)GetPedFromRef(GetPlayerActor());

    CVehicle* vehicle = nullptr;

    if (g_lastPlayerVehicle > 0 && CAR_DEFINED(g_lastPlayerVehicle)) {
        vehicle = (CVehicle*)GetVehicleFromRef(g_lastPlayerVehicle);

        if (vehicle) {
            float distanceToPed = distanceBetweenPoints(((CPlaceable*)vehicle)->GetPosition(), ((CPlaceable*)player)->GetPosition());

            if (distanceToPed > 200.0f) vehicle = nullptr;
        }
    }

    if (vehicle) {
        g_currentAudio->SetVolume(1.0f);
        g_currentAudio->AttachToCPlaceable((CPlaceable*)vehicle);
    } else {
        g_currentAudio->SetVolume(0.0f);
        //g_currentAudio->AttachToCPlaceable(player);
    }
}

void RadioSounds::PlayAudioNow(IAudio* audio) {
    if (g_currentAudio) { g_currentAudio->Stop(); }

    g_currentAudio = audio;

    AttachAudio();

    g_currentAudio->Play();
}

void RadioSounds::PlayAudioNowDontAttach(IAudio* audio) {
    if (g_currentAudio) { g_currentAudio->Stop(); }

    g_currentAudio = audio;

    //AttachAudio();

    g_currentAudio->SetVolume(1.0f);
    g_currentAudio->Play();
}