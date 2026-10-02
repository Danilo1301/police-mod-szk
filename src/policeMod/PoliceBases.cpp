#include "PoliceBases.h"
#include "../utils/iniReaderWriter.hpp"
#include "Checkpoint.h"

std::vector<PoliceBase*> PoliceBases::Bases;

void PoliceBases::Initialize()
{
    // auto base = new PoliceBase();
    // base->basePosition = CVector(1539.3256, -1688.3606, 13.5469);
    // base->leaveCriminalPosition = CVector(1539.3256, -1688.3606, 13.5469);

    // base->leaveCriminalCheckpoint->position = base->leaveCriminalPosition;

    // Bases.push_back(base);

    LoadBases();
}

void PoliceBases::LoadBases()
{
    std::string basesFolder = modData->GetFile("data/bases");

    // Se a pasta não existir, sair
    if (!std::filesystem::exists(basesFolder)) { return; }

    // Iterar pelos arquivos
    for (auto& entry : std::filesystem::directory_iterator(basesFolder))
    {
        // Aceitar apenas arquivos .ini
        if (!entry.is_regular_file()) continue;

        auto path = entry.path();
        if (path.extension() != ".ini") continue;

        // Ler o INI
        IniReaderWriter ini;
        if (!ini.LoadFromFile(path.string())) { continue; }

        auto base = new PoliceBase();

        // Ler basePosition
        float px = ini.GetDouble("base", "base_position_x", 0);
        float py = ini.GetDouble("base", "base_position_y", 0);
        float pz = ini.GetDouble("base", "base_position_z", 0);

        base->basePosition = CVector(px, py, pz);

        // Ler leaveCriminalPosition
        float lx = ini.GetDouble("base", "leave_criminal_position_x", 0);
        float ly = ini.GetDouble("base", "leave_criminal_position_y", 0);
        float lz = ini.GetDouble("base", "leave_criminal_position_z", 0);

        base->leaveCriminalPosition = CVector(lx, ly, lz);

        if (base->leaveCriminalCheckpoint) base->leaveCriminalCheckpoint->position = base->leaveCriminalPosition;

        {
            float x = ini.GetDouble("base", "get_partner_position_x", 0);
            float y = ini.GetDouble("base", "get_partner_position_y", 0);
            float z = ini.GetDouble("base", "get_partner_position_z", 0);

            base->getPartnerPosition = CVector(x, y, z);

            if (base->getPartnerCheckpoint) base->getPartnerCheckpoint->position = base->getPartnerPosition;
        }

        base->Init();

        Bases.push_back(base);
    }
}

void PoliceBases::Update()
{
    for (auto base : Bases) { base->Update(); }
}

void PoliceBases::OnPostDrawRadar()
{
    for (auto base : Bases) { base->OnPostDrawRadar(); }
}

void PoliceBases::OnDraw()
{
    for (auto base : Bases) { base->OnDraw(); }
}