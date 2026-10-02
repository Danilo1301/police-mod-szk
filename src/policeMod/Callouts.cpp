#include "Callouts.h"

#include "BottomMessage.h"
#include "opcodeCaller/CleoFunctions.h"
#include "Criminals.h"
#include "audio/AudioCollection.h"
#include "RadioSounds.h"
#include "callouts/CalloutRegistry.h"
#include "callouts/ATMCallout.h"
#include "callouts/StolenCarCallout.h"
#include "callouts/DrugDealerCallout.h"
#include "Ped.h"
#include "src/globals.h"

bool g_onACallout = false;
bool g_calloutReached = false;
int g_receiveCalloutTimer = 0;

static CalloutBase* g_broadcastingCallout = nullptr;
static CalloutBase* g_currentCallout = nullptr;
static CalloutBase* g_lastBroadcastedCallout = nullptr;

bool g_spawnedCriminals = false;

void Callouts::Initialize()
{
    CalloutRegistry::Register(new ATMCallout());
    CalloutRegistry::Register(new StolenCarCallout());
    CalloutRegistry::Register(new DrugDealerCallout());
}

void Callouts::Update()
{
    if (g_broadcastingCallout == nullptr && !g_onACallout) { g_receiveCalloutTimer += g_deltaTime; }

    if (!g_onACallout)
    {
        g_spawnedCriminals = false;
        g_calloutReached = false;
    }
    else
    {

        auto criminalsCount = Criminals::GetCriminals()->size();

        if (criminalsCount > 0 && g_calloutReached) { g_spawnedCriminals = true; }

        if (g_spawnedCriminals && criminalsCount == 0 && g_calloutReached)
        {
            g_onACallout = false;
            BottomMessage::AddMessage(TT("callout_ended"), 3000);
        }
    }

    if (g_onACallout)
    {
        if (ACTOR_DEAD(GetPlayerActor()))
        {
            g_onACallout = false;

            BottomMessage::SetMessage(TT("player_died_during_callout"), 2000);

            WAIT(500,
                []()
                {
                    auto criminals = *Criminals::GetCriminals();

                    for (auto criminal : criminals)
                    {
                        Criminals::RemoveCriminal(criminal);
                        criminal->QueueDestroy();
                    }
                });
        }
    }
}

void Callouts::TryBroadcastCallout()
{
    if (g_receiveCalloutTimer >= (g_secondsBetweenCallouts * 1000)) { BroadcastRandomCallout(); }
}

void Callouts::BroadcastRandomCallout()
{
    auto callout = CalloutRegistry::GetRandom();

    BroadcastCallout(callout);
}

void Callouts::BroadcastCallout(CalloutBase* callout)
{
    g_receiveCalloutTimer = 0;

    if (!callout) return;

    g_broadcastingCallout = callout;
    g_lastBroadcastedCallout = callout;

    BottomMessage::SetMessage(g_broadcastingCallout->GetBroadcastMessage(), 8000);

    auto audioGroup = g_broadcastingCallout->GetBroadcastAudio();
    auto audio = audioGroup->GetRandomAudio();

    if (DISABLE_CALLOUTS_RADIO_SOUND)
    {
        WAIT(5000, []() { g_broadcastingCallout = nullptr; });
    }
    else
    {
        RadioSounds::PlayAudioNow(audio);

        WaitForAudioFinish(audio, []() { WAIT(1000, []() { g_broadcastingCallout = nullptr; }); });
    }
}

void Callouts::AcceptCallout()
{
    auto callout = g_lastBroadcastedCallout;

    if (callout == nullptr) return;

    g_onACallout = true;

    g_onACallout = true;
    g_currentCallout = callout;
    g_lastBroadcastedCallout = nullptr;
    g_broadcastingCallout = nullptr;

    g_currentCallout->OnAccept();
}

bool Callouts::HasCalloutToAccept()
{
    return g_lastBroadcastedCallout != nullptr;
}

bool Callouts::IsBroacastingCallout()
{
    return g_broadcastingCallout != nullptr;
}

bool Callouts::IsInCallout()
{
    return g_onACallout;
}

void Callouts::OpenAcceptMenu(std::function<void(bool)> callback)
{
    auto window = CreatePM_Window(GetTranslatedText("window_accept_callout"), "Callout", 800);

    window->SetCloseButtonVisible(false);

    window->AddItem(GetTranslatedText("accept_or_reject_callout"));

    window->AddButton("~g~" + GetTranslatedText("accept"),
        [window, callback]()
        {
            AcceptCallout();
            window->Close();

            callback(true);
        });

    window->AddButton("~r~" + GetTranslatedText("cancel"),
        [window, callback]()
        {
            g_lastBroadcastedCallout = nullptr;
            g_broadcastingCallout = nullptr;

            window->Close();

            callback(false);
        });
}