#include "Peds.h"

#include "../hooks.h"
#include "opcodeCaller/CleoFunctions.h"

std::map<int, Ped*> Peds::peds;

void Peds::AddPed(int ref, void* ptr)
{
    if (peds.find(ref) != peds.end())
    {
        // já existe
        return;
    }

    auto ped = new Ped(ref, ptr);
    peds[ref] = ped;
}

Ped* Peds::RegisterPed(int ref)
{
    if (peds.find(ref) != peds.end())
    {
        // já existe
        return NULL;
    }

    auto ptr = GetPedFromRef(ref);

    auto ped = new Ped(ref, ptr);
    peds[ref] = ped;

    return ped;
}

void Peds::RemovePed(int ref)
{
    // verifica se existe
    auto it = peds.find(ref);
    if (it == peds.end()) return; // não existe, sai

    // pega o ped
    Ped* ped = it->second;

    // remove do map
    peds.erase(it);

    // deleta o objeto
    delete ped;
}

bool Peds::IsValid(Ped* ped)
{
    if (!ped) return false; // ponteiro nulo nunca é válido

    for (const auto& pair : peds)
    {
        if (pair.second == ped) // encontrou o ponteiro
        {
            return ACTOR_DEFINED(ped->ref);
        }
    }

    return false; // não encontrado
}

Ped* Peds::GetPed(int ref)
{
    if (ref < 0) return nullptr;

    auto it = peds.find(ref);
    if (it == peds.end()) return nullptr; // não existe

    return it->second; // existe, retorna
}

std::map<int, Ped*> Peds::GetPedsMap()
{
    return peds;
}

void Peds::Update()
{
    auto pedsCopy = peds;

    // percorre todos os peds no map
    for (auto& pair : pedsCopy)
    {
        Ped* ped = pair.second;

        if (!ped) continue;

        if (!ACTOR_DEFINED(ped->ref)) continue;

        // chama o update individual do ped
        ped->Update();
    }
}