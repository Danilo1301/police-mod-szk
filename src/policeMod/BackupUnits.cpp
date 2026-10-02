#include "BackupUnits.h"

#include "BottomMessage.h"
#include "Criminals.h"
#include "ModelLoader.h"
#include "Peds.h"
#include "RadioSounds.h"
#include "Vehicles.h"
#include "ai/AIController.h"
#include "ai/AICop.h"
#include "ai/AIHelicopterCop.h"
#include "ai/AIMedic.h"
#include "ai/AIPed.h"
#include "audio/AudioCollection.h"
#include "mod/logger.h"
#include "opcodeCaller/CleoFunctions.h"
#include "src/globals.h"

const float MAX_DISTANCE_TO_QTH = 300.0f;

bool g_hasInformedRadio = false;

CVector g_lastQTHPosition = CVector(0, 0, 0);
int g_spawnUnitTimer = 0;

std::vector<RoadName> g_roads;

std::vector<Vehicle*> BackupUnits::backupVehicles;

void BackupUnits::Initialize()
{
    InitializeRoads();
}

void BackupUnits::InitializeRoads()
{
    std::string roadDataPath = modData->GetFileFromAssets("roads/roads.dat");

    std::ifstream file(roadDataPath);
    if (!file.is_open()) { return; }

    std::string line;
    while (std::getline(file, line))
    {
        // Ignora linhas vazias ou comentários
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string audioFile;
        float x, y;

        // Faz o parsing: audioFile, posX, posY
        if (!(std::getline(ss, audioFile, ',') >> x))
        {
            // Se não conseguir ler corretamente, tenta com parsing mais robusto
            char comma;
            ss.clear();
            ss.str(line);
            if (!(std::getline(ss, audioFile, ',') && ss >> x >> comma >> y)) { continue; }
        }
        else
        {
            // quando o primeiro getline dá certo, precisa ler os outros 2 valores
            char comma;
            ss >> comma >> y;
        }

        // Remove espaços em branco do nome do arquivo
        audioFile.erase(0, audioFile.find_first_not_of(" \t"));
        audioFile.erase(audioFile.find_last_not_of(" \t") + 1);

        // Cria o grupo de áudio
        auto group = AudioCollection::CreateGroup(audioFile);

        std::string fullAudioPath = modData->GetFileFromAssets("roads/audios/" + audioFile);
        group->LoadNewAudio(fullAudioPath);

        // Cria o objeto da rua
        RoadName road;
        road.audioGroup = group;
        road.position = CVector(x, y, 0.0f);

        g_roads.push_back(road);
    }

    file.close();
}

void BackupUnits::Update()
{
    CheckIfVehiclesAreValid();

    auto criminalsCount = Criminals::GetCriminals()->size();

    if (criminalsCount == 0)
    {
        g_lastQTHPosition = CVector(0, 0, 0);
        g_hasInformedRadio = false;
    }

    auto distanceFromQTH = distanceBetweenPoints(GetPlayerPosition(), g_lastQTHPosition);

    if (distanceFromQTH < MAX_DISTANCE_TO_QTH)
    {
        if (criminalsCount > 0 && backupVehicles.size() < 5)
        {
            g_spawnUnitTimer += g_deltaTime;

            if (g_spawnUnitTimer >= 8000)
            {
                g_spawnUnitTimer = 0;

                SpawnRandomBackupUnit();
            }
        }
    }
}

void BackupUnits::OnPostDrawRadar()
{
    if (textureBigCircle == nullptr) return;

    auto playerPosition = GetPlayerPosition();
    auto distance = distanceBetweenPoints(g_lastQTHPosition, playerPosition);

    const float fadeStart = MAX_DISTANCE_TO_QTH - 80.0f;
    const float fadeEnd = MAX_DISTANCE_TO_QTH;

    unsigned char alpha = 0;

    if (distance <= fadeStart)
    {
        alpha = 100; // totalmente visível até 200m
    }
    else if (distance >= fadeEnd)
    {
        alpha = 0; // invisível a partir de 300m
    }
    else
    {
        // interpolar suavemente entre 200 e 300
        float t = (distance - fadeStart) / (fadeEnd - fadeStart);
        alpha = static_cast<unsigned char>(100 * (1.0f - t));
    }

    auto color = COLOR_POLICE;
    color.a = alpha;

    auto radarPoint = WorldToRadarPoint(playerPosition);

    menuSZK->DrawTextureOnRadar(textureBigCircle, radarPoint, color, 150.0f);
}

void BackupUnits::CheckIfVehiclesAreValid()
{
    backupVehicles.erase(std::remove_if(backupVehicles.begin(),
                             backupVehicles.end(),
                             [](Vehicle* vehicle)
                             {
                                 // Remove se não for válido
                                 return !Vehicles::IsValid(vehicle);
                             }),
        backupVehicles.end());
}

void BackupUnits::SendQTH()
{
    logger->Info("BackupUnits: SendQTH");

    if (Criminals::GetCriminals()->size() == 0)
    {
        BottomMessage::SetMessage(GetTranslatedText("error_no_suspects_found"), 3000);
        return;
    }

    g_lastQTHPosition = GetPlayerPosition();

    BottomMessage::SetMessage(GetTranslatedText("qth_updated"), 3000);

    if (g_hasInformedRadio == false)
    {
        auto audio = audioRequestBackup->GetRandomAudio();

        RadioSounds::PlayAudioNow(audio);

        // WaitForAudioFinish(audio, []() {
        //     PlayRoadName();
        // });
    }
    else
    {
        PlayRoadName();
    }

    g_hasInformedRadio = true;

    // SpawnBackupUnit();
}

void BackupUnits::PlayRoadName()
{
    if (g_roads.empty()) return;

    CVector myPosition = GetPlayerPosition();

    RoadName* nearestRoad = nullptr;
    float nearestDistance = FLT_MAX;

    for (auto& road : g_roads)
    {
        float dx = road.position.x - myPosition.x;
        float dy = road.position.y - myPosition.y;
        float dz = road.position.z - myPosition.z;

        float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestRoad = &road;
        }
    }

    if (nearestRoad && nearestRoad->audioGroup)
    {
        auto audio = nearestRoad->audioGroup->GetRandomAudio();

        RadioSounds::PlayAudioNow(audio);
    }
}

void BackupUnits::SpawnBackupUnit(PoliceVehicleData* unit)
{
    logger->Info("BackupUnits: SpawnBackupUnit");

    if (Criminals::GetCriminals()->size() == 0)
    {
        BottomMessage::SetMessage(GetTranslatedText("error_no_suspects_found"), 3000);
        return;
    }

    bool isHelicopter = IsModelAPoliceHelicopter(unit->vehicleModelId);

    auto closePosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 120, 0));

    auto spawnPosition = GET_CLOSEST_CAR_NODE(closePosition.x, closePosition.y, closePosition.z);

    if (isHelicopter) { spawnPosition.z += 100.0f; }

    ModelLoader::AddModelToLoad(unit->vehicleModelId);
    ModelLoader::AddModelToLoad(unit->skinModelId);
    ModelLoader::LoadAll(
        [unit, spawnPosition, isHelicopter]()
        {
            auto carRef = CREATE_CAR_AT(unit->vehicleModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
            auto car = Vehicles::RegisterVehicle(carRef);

            auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carRef, PedType::Special, unit->skinModelId);
            Peds::RegisterPed(driverRef);

            auto numOfPassengers = unit->occupants - 1;

            if (numOfPassengers > 0)
            {
                for (auto i = 0; i < numOfPassengers; i++)
                {
                    auto passengerRef = CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(carRef, PedType::Special, unit->skinModelId, i);
                    Peds::RegisterPed(passengerRef);
                }
            }

            if (isHelicopter)
            {
                SET_HELICOPTER_INSTANT_ROTOR_START(carRef);
                SET_CAR_ENGINE_OPERATION(carRef, true);
            }

            AddVehicleAsBackup(car, false);
        });
}

void BackupUnits::SpawnRandomBackupUnit()
{
    logger->Info("BackupUnits: SpawnRandomBackupUnit");

    auto unit = GetRandomUnitByChance(PoliceVehicles::vehicles);

    SpawnBackupUnit(unit);
}

void BackupUnits::SpawnHelicopterBackup()
{
    logger->Info("BackupUnits: SpawnHelicopterBackup");

    auto unit = GetRandomUnitByChance(PoliceVehicles::helicopters);

    SpawnBackupUnit(unit);
}

void BackupUnits::AddVehicleAsBackup(Vehicle* vehicle, bool recreatePeds)
{
    logger->Info("BackupUnits: AddVehicleAsBackup");

    // REMOVE_REFERENCES_TO_CAR(vehicle->ref);

    if (recreatePeds)
    {
        auto oldDriverRef = vehicle->GetCurrentDriver();

        if (oldDriverRef > 0)
        {
            auto oldDriver = Peds::GetPed(oldDriverRef);

            auto driverModelId = GET_ACTOR_MODEL(oldDriver->ref);

            oldDriver->DestroyImmediate();

            auto newDriverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(vehicle->ref, PedType::Special, driverModelId);
            Peds::RegisterPed(newDriverRef);
        }
    }

    logger->Info("Setup occupants");

    bool isHelicopter = IsModelAPoliceHelicopter(vehicle->GetModelId());

    auto occupants = vehicle->GetCurrentOccupants();
    for (auto pedRef : occupants)
    {
        auto cop = Peds::GetPed(pedRef);

        ModelLoader::AddModelToLoad(346);
        ModelLoader::LoadAll([pedRef]() { GIVE_ACTOR_WEAPON(pedRef, 22, 100); });

        // cop->ShowBlip(COLOR_POLICE);

        if (isHelicopter)
        {
            auto ai = new AIHelicopterCop();
            AIController::AddAIToPed(cop, ai);
            ai->Start();
        }
        else
        {
            auto ai = new AICop();
            AIController::AddAIToPed(cop, ai);
            ai->Start();
        }
    }

    vehicle->SetOwners();
    vehicle->ShowBlip(COLOR_POLICE);

    backupVehicles.push_back(vehicle);
}

PoliceVehicleData* BackupUnits::GetRandomUnitByChance(std::vector<PoliceVehicleData>& units)
{
    if (units.empty()) return nullptr;

    // Calcula o total de chances (somatório)
    float totalChance = 0.0f;
    for (auto& unit : units) totalChance += unit.chance; // supondo que BackupUnit tenha 'float chance'

    if (totalChance <= 0.0f) return nullptr;

    // Gera um número aleatório entre 0 e totalChance
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, totalChance);

    float randomValue = dist(gen);

    // Escolhe a unit com base nas chances acumuladas
    float cumulative = 0.0f;
    for (auto& unit : units)
    {
        cumulative += unit.chance;
        if (randomValue <= cumulative) return &unit;
    }

    // fallback (em teoria nunca chega aqui)
    return &units.back();
}

void BackupUnits::OpenSpawnBackupMenu()
{
    auto window = CreatePM_Window(GetTranslatedText("window_spawn_backup"), "Spawn backup", 800);

    for (auto& unit : PoliceVehicles::vehicles)
    {
        PoliceVehicleData unitCopy = unit;

        auto button = window->AddButton(unit.name,
            [window, unitCopy]() mutable
            {
                window->Close();
                SpawnBackupUnit(&unitCopy);
            });
    }

    for (auto& unit : PoliceVehicles::helicopters)
    {
        PoliceVehicleData unitCopy = unit;

        auto button = window->AddButton(unit.name,
            [window, unitCopy]() mutable
            {
                window->Close();
                SpawnBackupUnit(&unitCopy);
            });
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"), [window]() { window->Close(); });
    }
}

void BackupUnits::SpawnMedicUnit(CVector position)
{
    auto vehicleModelId = 416;
    auto pedModelId = 274;

    auto spawnPosition = GET_CLOSEST_CAR_NODE(position.x + 120, position.y, position.z);

    ModelLoader::AddModelToLoad(vehicleModelId);
    ModelLoader::AddModelToLoad(pedModelId);
    ModelLoader::LoadAll(
        [vehicleModelId, pedModelId, spawnPosition, position]()
        {
            auto carRef = CREATE_CAR_AT(vehicleModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
            auto car = Vehicles::RegisterVehicle(carRef);

            auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carRef, PedType::Special, pedModelId);
            Peds::RegisterPed(driverRef);

            auto passengerRef = CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(carRef, PedType::Special, pedModelId, 0);
            Peds::RegisterPed(passengerRef);

            car->SetOwners();
            car->ShowBlip(COLOR_YELLOW);

            for (auto pedRef : car->GetCurrentOccupants())
            {
                auto medic = Peds::GetPed(pedRef);

                auto ai = new AIMedic();
                AIController::AddAIToPed(medic, (AIPed*)ai);
                ai->targetPosition = position;
                ai->Start();
            }
        });
}