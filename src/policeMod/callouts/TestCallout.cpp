#include "TestCallout.h"

#include "../audio/AudioCollection.h"
#include "../RadioSounds.h"
#include "../BottomMessage.h"
#include "../ScriptTask.h"
#include "../opcodeCaller/CleoFunctions.h"
#include "../ATMSystem.h"
#include "../ModelLoader.h"
#include "../Peds.h"
#include "../Criminals.h"
#include "../Callouts.h"

extern bool g_calloutReached;

std::string TestCallout::GetBroadcastMessage()
{
    return "~y~[COPOM] ~w~Denuncia de alguma coisa n sei bem oq e";
}

AudioVariationGroup* TestCallout::GetBroadcastAudio()
{
    return audioCalloutStolenCar;
}

void TestCallout::OnAccept()
{
    BottomMessage::SetMessage(GetTranslatedText("go_to_local_on_map"), 3000);

    auto location = ATMSystem::GetATMLocation(GetPlayerPosition());
    auto position = location->position;

    int marker = CreateMarker(position.x, position.y, position.z, 0, 3, 3);

    ScriptTask* taskAproach = new ScriptTask("aproach_callout");
    taskAproach->onBegin = []() {

    };
    taskAproach->onExecute = [location]()
    {
        auto playerPosition = GetPlayerPosition();
        auto distance = distanceBetweenPoints(playerPosition, location->position);

        if (distance > 150.0f) return SCRIPT_KEEP_GOING;

        return SCRIPT_SUCCESS;
    };
    taskAproach->onComplete = [marker, location]()
    {
        g_calloutReached = true;

        DISABLE_MARKER(marker);

        BottomMessage::SetMessage("~r~Detenha os suspeitos", 4000);

        std::vector<int> criminalSkins;
        for (int i = 0; i < 4; i++)
        {
            auto skin = GetRandomCriminalSkin();

            criminalSkins.push_back(skin);
            ModelLoader::AddModelToLoad(skin);
        }

        int coltId = 22;
        int coltModel = 346;

        ModelLoader::AddModelToLoad(coltModel);
        ModelLoader::LoadAll(
            [location, criminalSkins, coltId]()
            {
                for (int i = 0; i < 4; i++)
                {
                    auto skinModel = criminalSkins[i];

                    auto pedRef = SpawnPedRandomlyAtPosition_PedNode(location->position, PedType::CivMale, skinModel, 50.0f);
                    auto criminal = Peds::RegisterPed(pedRef);

                    Criminals::AddCriminal(criminal);

                    criminal->ShowBlip(COLOR_CRIMINAL);

                    GIVE_ACTOR_WEAPON(pedRef, coltId, 1000);
                    SET_ACTOR_HEALTH(criminal->ref, 500);

                    criminal->flags.willSurrender = false;

                    if (i >= 1) { criminal->flags.willKillCops = true; }
                    else
                    {
                        criminal->flags.willKillCops = false;
                    }
                }
            });
    };
    taskAproach->Start();
}