#include "Criminals.h"

#include "Peds.h"
#include "ai/AIController.h"
#include "ai/AICriminal.h"
#include "opcodeCaller/CleoFunctions.h"

std::vector<Ped *> criminals;

void Criminals::Update() {}

void Criminals::CheckIfCriminalsAreValid()
{
    // Remove todos os criminosos que não existem mais
    criminals.erase(std::remove_if(criminals.begin(), criminals.end(),
                        [](Ped *criminal) { return (criminal == nullptr || !ACTOR_DEFINED(criminal->ref)); }),
        criminals.end());
}

void Criminals::AddCriminal(Ped *ped)
{
    if (IsCriminal(ped))
        return;

    criminals.push_back(ped);

    auto ai = new AICriminal();

    AIController::AddAIToPed(ped, ai);

    ai->Start();
}

void Criminals::RemoveCriminal(Ped *ped)
{
    if (!IsCriminal(ped))
        return;

    auto it = std::find(criminals.begin(), criminals.end(), ped);

    criminals.erase(it);

    AIController::RemoveAIsFromPed(ped);
}

bool Criminals::IsCriminal(Ped *ped)
{
    if (ped == nullptr)
        return false;

    auto it = std::find(criminals.begin(), criminals.end(), ped);

    return it != criminals.end();
}

std::vector<Ped *> *Criminals::GetCriminals() { return &criminals; }

AICriminal *Criminals::GetAIOfPed(Ped *ped)
{
    auto it = AIController::pedsAI.find(ped);
    if (it == AIController::pedsAI.end() || it->second == nullptr)
        return nullptr;

    return static_cast<AICriminal *>(it->second);
}