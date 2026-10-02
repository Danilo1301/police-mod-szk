#include "DrugDealerCallout.h"

#include "../audio/AudioCollection.h"
#include "../BottomMessage.h"
#include "../opcodeCaller/CleoFunctions.h"
#include "../ScriptTask.h"
#include "../ModelLoader.h"
#include "../Biqueiras.h"
#include "../Peds.h"
#include "../inventory/InventoryItemManager.h"

extern bool g_calloutReached;

std::string DrugDealerCallout::GetBroadcastMessage()
{
    return TT("callout_drug_dealer");
}

AudioVariationGroup* DrugDealerCallout::GetBroadcastAudio()
{
    return audioCalloutDrugDealer;
}

void DrugDealerCallout::OnAccept()
{
    BottomMessage::SetMessage(TT("go_to_report"), 3000);

    auto location = Biqueiras::GetRandomCloseBiqueira(GetPlayerPosition());

    int marker = CreateMarker(location.position.x, location.position.y, location.position.z, 0, 3, 3);

    ScriptTask* taskAproach = new ScriptTask("aproach_callout");
    taskAproach->onBegin = []() {

    };
    taskAproach->onExecute = [location]()
    {
        auto playerPosition = GetPlayerPosition();
        auto distance = distanceBetweenPoints(playerPosition, location.position);

        if (distance > 100.0f) return SCRIPT_KEEP_GOING;

        return SCRIPT_SUCCESS;
    };
    taskAproach->onComplete = [marker, location]()
    {
        g_calloutReached = true;

        DISABLE_MARKER(marker);

        BottomMessage::SetMessage(TT("aproach_the_suspect"), 4000);

        auto skin = GetRandomCriminalSkin();

        ModelLoader::AddModelToLoad(skin);
        ModelLoader::LoadAll(
            [skin, location]()
            {
                auto dealerRef =
                    CREATE_ACTOR_PEDTYPE(PedType::Special, skin, location.position.x, location.position.y, location.position.z);
                auto dealer = Peds::RegisterPed(dealerRef);

                dealer->ShowBlip(COLOR_CRIMINAL);

                dealer->flags.willSurrender = calculateProbability(0.20);

                if (dealer->flags.willKillCops == false) { dealer->flags.willKillCops = calculateProbability(0.20); }

                for (auto item : InventoryItemManager::itemDefinitions)
                {
                    auto id = item.first;
                    auto itemDef = *item.second;

                    if (!ItemDefinitionContainFlag(itemDef, "used_by_drug_dealer")) continue;

                    if (calculateProbability(itemDef.chance))
                    {
                        int amount = getRandomNumber(1, itemDef.maxAmount);

                        dealer->inventory.AddItem(id, amount);
                    }
                }

                ScriptTask* taskRemoveRef = new ScriptTask("taskRemoveRef");
                taskRemoveRef->onExecute = [location]()
                {
                    auto playerPosition = GetPlayerPosition();
                    auto distance = distanceBetweenPoints(playerPosition, location.position);

                    if (distance > 15.0f) return SCRIPT_KEEP_GOING;

                    return SCRIPT_SUCCESS;
                };
                taskRemoveRef->onComplete = [dealerRef]()
                {
                    if (ACTOR_DEFINED(dealerRef))
                    {
                        CLEAR_ACTOR_TASK(dealerRef);
                        REMOVE_REFERENCES_TO_ACTOR(dealerRef);
                    }
                };
                taskRemoveRef->Start();
            });
    };
    taskAproach->Start();
}