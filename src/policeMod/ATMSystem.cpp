#include "ATMSystem.h"

#include "opcodeCaller/CleoFunctions.h"
#include "ModelLoader.h"

#include "../utils/utils.h"

const int ATM_ID = 2942;
const float OFFSET_TO_FIX_TS = 0.8f;

std::vector<Location*> g_atmPositions;

void ATMSystem::Initialize() {
    //ls
    g_atmPositions.push_back(new Location{ CVector(1192.14, -1782.2, 13.2984), CVector(0, 0, 180) });
    g_atmPositions.push_back(new Location{ CVector(1729.3, -1745.94, 13.1297), CVector(0, 0, -90) });
    g_atmPositions.push_back(new Location{ CVector(1186.21, -1379.96, 13.2072), CVector(0, 0, 270) });

    //sf
    g_atmPositions.push_back(new Location{ CVector(-2649.93, 389.17, 3.99844), CVector(0, 0, 90) });
    g_atmPositions.push_back(new Location{ CVector(-1622.39, 1165.57, 6.85), CVector(0, 0, 230) });

    //lv
    g_atmPositions.push_back(new Location{ CVector(1715.83, 1533.62, 10.4984), CVector(0, 0, 270) });
    g_atmPositions.push_back(new Location{ CVector(2182.63, 2123.69, 10.4938), CVector(0, 0, 180) });

    ModelLoader::AddModelToLoad(ATM_ID);
    ModelLoader::LoadAll([]() { CreateObjects(); });
}

void ATMSystem::CreateObjects() {
    for (auto location : g_atmPositions) {
        auto position = location->position;

        auto objectId = CREATE_OBJECT(ATM_ID, position.x, position.y, position.z - OFFSET_TO_FIX_TS);
        SET_OBJECT_Z_ANGLE(objectId, location->rotation.z);
        SET_OBJECT_PROOFS(objectId, true, true, true, true, true);
    }
}

Location* ATMSystem::GetRandomATMLocation() {
    if (g_atmPositions.empty()) return nullptr;

    // inicializa o gerador de números aleatórios uma vez (pode colocar no construtor)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int randomIndex = std::rand() % g_atmPositions.size();

    return g_atmPositions[randomIndex];
}

Location* ATMSystem::GetATMLocation(CVector position) {
    if (g_atmPositions.empty()) return nullptr;

    struct DistItem {
        Location* loc;
        float dist;
    };

    std::vector<DistItem> list;

    // calcular distâncias de todos
    for (auto* atm : g_atmPositions) {
        float dist = CVectorDistance(atm->position, position);
        list.push_back({ atm, dist });
    }

    // ordenar ATMs por distância crescente
    std::sort(list.begin(), list.end(), [](const DistItem& a, const DistItem& b) { return a.dist < b.dist; });

    // limitar para os 3 mais próximos (ou menos se não tiver 3)
    int count = std::min((int)list.size(), 3);

    // escolher aleatório entre eles
    int r = std::rand() % count;
    return list[r].loc;
}