#pragma once

#include "../pch.h"

#include "../utils/utils.h"

#include "inventory/Inventory.h"

class Checkpoint;
class WorldWidget;

struct ePedFlags
{
    bool hasSurrended = false;
    bool willSurrender = true;
    bool willKillCops = false;

    bool isInconcious = false;
    bool showWidget = false;
    bool beeingEscorted = false;
    int copActingOnPed = 0;
    bool shownRG = false;
    bool wantedByJustice = false;
    bool isBeingTreated = false;
    int targetPed = -1;

    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;

    bool showBackCheckpoint = false;
    bool canShowFriskMenu = false;
    bool hasDoneWork = false;
};

class Ped
{
private:
    bool wasAlive = false;

    std::string currentAnim = "";
    std::string currentAnimGroup = "";

public:
    int ref;
    void* ptr;

    bool isLeavingCar = false;
    bool isEnteringCar = false;

    ePedFlags flags;

    IWidget* widgetOptions = nullptr;
    WorldWidget* worldWidget = nullptr;

    int prevSeatId = -1;
    int previousVehicle = -1;

    int vehicleOwned = -1;

    std::string rg;
    std::string cpf;
    std::string birthDate;
    std::string name;

    std::string catHab;
    std::string cnhExpireDate;

    Inventory inventory;

    Checkpoint* backCheckpoint = nullptr;

    Ped(int ref, void* ptr);
    ~Ped();

    void Update();

    void SetCanDoHandsup();
    void PerformAnims();
    bool IsPerformingAnimation(const std::string& animName);

    void ShowBlip(CRGBA color);
    void HideBlip();

    CVector GetPosition();
    void SetPosition(CVector position);

    bool IsInAnyCar();
    int GetCurrentCar();
    bool IsDriver();
    void LeaveCar();

    void UpdateSeatPosition();

    void EnterVehicle(int vehicleRef, int seatId);
    void EnterPreviousVehicle();

    void StartDrivingRandomly();

    void SetAnim(std::string anim, std::string animGroup);
    void ClearAnim();

    void InitializeOnVehicle(int vehicleRef);

    void CopyFrom(const Ped& other);

    void Reanimate();

    void DestroyImmediate();
    void QueueDestroy();

    bool IsDeadOrInconcious();

    void TryInitializeInventory();

    void OnEnterBackCheckpoint();
};

inline std::string randomRG()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::stringstream ss;
    ss << dist(gen) << dist(gen) << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-" << dist(gen);

    return ss.str();
}

inline std::string randomCPF()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::stringstream ss;
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 2; ++i) ss << dist(gen);

    return ss.str();
}

inline std::string randomCatHab()
{
    std::string cat;

    // 50% chance de ter A
    if (calculateProbability(0.50f)) cat += "A";

    // 90% chance de ter B
    if (calculateProbability(0.90f))
    {
        cat += "B";

        // 8% chance de ter C
        if (calculateProbability(0.08f)) cat += "C";

        // 3% chance de ter D
        if (calculateProbability(0.03f)) cat += "D";

        // 3% chance de ter E
        if (calculateProbability(0.03f)) { cat += "E"; }
    }

    if (cat.empty()) cat = "B"; // fallback, sempre tem pelo menos B

    return cat;
}

inline std::string randomBirthDate(int minYear, int maxYear)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dayDist(1, 28); // evita complicação de fevereiro
    std::uniform_int_distribution<> monthDist(1, 12);
    std::uniform_int_distribution<> yearDist(minYear, maxYear);

    int day = dayDist(gen);
    int month = monthDist(gen);
    int year = yearDist(gen);

    std::stringstream ss;
    ss << (day < 10 ? "0" : "") << day << "/" << (month < 10 ? "0" : "") << month << "/" << year;

    return ss.str();
}

// Pega ano atual
inline int getCurrentYear()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    return now->tm_year + 1900;
}

// Retorna string no formato DD/MM/AAAA
inline std::string gerarValidadeCNH(int minYear, int maxYear)
{
    int ano = getRandomNumber(minYear, maxYear);
    int mes = getRandomNumber(1, 12);

    int diaMax;
    switch (mes)
    {
        case 2: // Fevereiro (ignora bissexto por simplicidade)
            diaMax = 28;
            break;
        case 4:
        case 6:
        case 9:
        case 11: // Abril, Junho, Setembro, Novembro
            diaMax = 30;
            break;
        default: diaMax = 31;
    }

    int dia = getRandomNumber(1, diaMax);

    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, ano);

    return std::string(buffer);
}