#include "Partners.h"

#include "BottomMessage.h"
#include "Weapons.h"
#include "Skins.h"
#include "opcodeCaller/CleoFunctions.h"
#include "Peds.h"
#include "ModelLoader.h"

int g_currentSelectedSkinIndex = 0;
int g_currentSelectedWeaponIndex = 0;

std::vector<Ped*> g_allPartners;

void Partners::CreateSpawnPartnerMenu()
{
    auto window = CreatePM_Window(GetTranslatedText("window_partners"), "Partners");

    {
        auto options = window->AddOptions("Skin ID");
        options->onValueChange->Add(
            [options]()
            {
                auto value = options->GetCurrentOptionValue();
                g_currentSelectedSkinIndex = value;
            });

        for (size_t i = 0; i < g_policeSkins.size(); i++)
        {
            auto skinInfo = g_policeSkins[i];

            options->AddOption(i, std::to_string(skinInfo.id) + ", " + skinInfo.name);
        }
        options->SetOptionIndex(0);
    }

    {
        auto options = window->AddOptions("Weapon ID");
        options->onValueChange->Add(
            [options]()
            {
                auto value = options->GetCurrentOptionValue();
                g_currentSelectedWeaponIndex = value;
            });

        for (size_t i = 0; i < g_weaponList.size(); i++)
        {
            auto weaponInfo = g_weaponList[i];
            options->AddOption(i, weaponInfo.name);
        }
        options->SetOptionIndex(0);
    }

    {
        auto button = window->AddButton(GetTranslatedText("spawn_partner"),
            [window]()
            {
                window->Close();

                auto spawnPosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 1, 0));

                SpawnPartner(spawnPosition);
            });
    }

    {
        auto button = window->AddButton(GetTranslatedText("remove_partners"),
            [window]()
            {
                window->Close();

                DestroyPartners();
            });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void Partners::SpawnPartner(CVector position)
{
    logger->Info("Partners: SpawnPartner");

    auto skinInfo = g_policeSkins[g_currentSelectedSkinIndex];
    auto weaponInfo = g_weaponList[g_currentSelectedWeaponIndex];

    //logger->Info("weapon %" + weaponInfo.name);
    //logger->Info("skin " + skinInfo.name);

    ModelLoader::AddModelToLoad(skinInfo.id);
    ModelLoader::AddModelToLoad(weaponInfo.modelId);

    ModelLoader::LoadAll(
        [skinInfo, weaponInfo, position]()
        {
            auto pedRef = CREATE_ACTOR_PEDTYPE(PedType::Gang2, skinInfo.id, position.x, position.y, position.z);
            auto ped = Peds::RegisterPed(pedRef);

            REMOVE_REFERENCES_TO_ACTOR(pedRef);

            g_allPartners.push_back(ped);

            auto playerActor = GET_PLAYER_ACTOR(0);
            auto playerGroup = GET_PLAYER_GROUP(0);

            PUT_ACTOR_IN_GROUP_AS_LEADER(pedRef, playerActor);
            PUT_ACTOR_IN_GROUP(playerGroup, pedRef);

            GIVE_ACTOR_WEAPON(pedRef, weaponInfo.weaponId, 10000);
            SET_ACTOR_HEALTH(pedRef, 500.0f);
        });
}

void Partners::DestroyPartners()
{
    for (auto ped : g_allPartners) { ped->QueueDestroy(); }

    g_allPartners.clear();
}