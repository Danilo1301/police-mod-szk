#pragma once

#include "ModData.h"
#include "Logger.h"

// CLEO 2.0.1.3
#include "cleo.h"
extern cleo_ifs_t* cleo;

// SAUtils 1.6
#include "isautils.h"
extern ISAUtils* sautils;

// MenuSZK
#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#define menuDebug menuSZK->debug

#define NO_PED_FOUND -1

#include "hooks.h"

#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "EventListener.h"

inline bool HasCleo() { return cleo != nullptr; }

inline bool blockInput = false;

inline void* textureBlip = nullptr;
inline void* textureCircle = nullptr;
inline void* textureBigCircle = nullptr;
inline void* texturePoliceDP = nullptr;

inline CVector* g_playerPosition = new CVector(0, 0, 0);
inline CVector2D g_defaultMenuPosition = CVector2D(400, 200);
inline int g_lastPlayerVehicle = -1;

inline EventListener<int>* g_onPedEnterVehicle = new EventListener<int>();
inline EventListener<int>* g_onPedLeaveVehicle = new EventListener<int>();
inline EventListener<int>* g_onVehicleDestroy = new EventListener<int>();

inline std::vector<int> g_vehiclesToDestroy;
inline std::vector<int> g_pedsToDestroy;

inline std::vector<int> g_criminalSkins = {20, 18, 28, 66, 108};

inline std::vector<int> g_stolenVehicleIds = {405, 414, 440, 456, 461, 522, 482, 496, 499, 507};

inline bool g_blockInteractions = false;

inline int CHASE_VEHICLE_MAX_SPEED = 50.0f;
inline int CHASE_POLICE_MAX_SPEED = 50.0f;
inline int CHASE_MIN_TIME_TO_SURRENDER = 40.0f;
inline bool CREATE_INJURED_PED = true;
inline bool DISABLE_CALLOUTS_RADIO_SOUND = false;

inline float CHANCE_CRIMINAL_KILL_COPS = 0.40f;

inline float CHANCE_RUNNING_AWAY_WHEN_VEHICLE_IRREGULAR = 0.30f;

inline float CHANCE_BEEING_WANTED_BY_JUSTICE = 0.10;

inline CVector2D g_widgetsStartPosition = CVector2D(450, 50);

inline void QueueDestroyPed(int ref)
{
    g_pedsToDestroy.push_back(ref);
}

inline void QueueDestroyVehicle(int ref)
{
    g_vehiclesToDestroy.push_back(ref);
}

#define COLOR_CRIMINAL CRGBA(255, 0, 0)
#define COLOR_YELLOW CRGBA(255, 255, 0)
#define COLOR_POLICE CRGBA(0, 150, 255)

template <typename T>
inline T* loadInterface(T** out, std::string name, bool optional = false)
{
    fileLog->Log("Looking for " + name + "...");

    void* interface = GetInterface(name.c_str());

    *out = static_cast<T*>(interface);

    if (*out)
    {
        fileLog->Log(name + " loaded");
    } else {

        if(optional)
        {
            fileLog->Log(name + " was not found, but its optional");
        } else {
            
            fileLog->Error(name + " was not found");
        }
    }

    return *out;
}

inline double distanceBetweenPoints(CVector point1, CVector point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;
    double dz = point1.z - point2.z;

    return sqrt( dx * dx + dy * dy + dz * dz );
}

inline double distanceBetweenPoints2D(CVector2D point1, CVector2D point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;

    return sqrt(dx * dx + dy * dy);
}

inline std::string VectorToString(const CVector& vec, int precision = 2)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return oss.str();
}

inline int getRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

inline bool calculateProbability(float chance)
{
    int i = getRandomNumber(0, 99);
    return i < (int)(chance * 100.0f);
}

inline int GetRandomCriminalSkin()
{
    if (g_criminalSkins.empty())
        return -1;

    int index = getRandomNumber(0, static_cast<int>(g_criminalSkins.size()) - 1);
    return g_criminalSkins[index];
}

inline bool file_exists(const std::string& path)
{
    std::ifstream f(path.c_str());
    return f.good();
}

inline std::string randomPlate()
{
    // Letras permitidas (pode adicionar/remover)
    static const std::vector<std::string> prefixes = {
        "EGC", "BRA", "MNT", "CPU", "LVR", "SJP", "FGT", "QXZ", "HJK", "VPE", 
        "NDO", "RSC", "YAB", "ZGM", "PQL", "TJS", "WFM", "LXN", "CBF", "GVN"
    };

    // RNG (random device + Mersenne Twister)
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Sorteia prefixo
    std::uniform_int_distribution<> prefixDist(0, prefixes.size() - 1);
    std::string plate = prefixes[prefixDist(gen)];

    // Sorteia números (0000–9999)
    std::uniform_int_distribution<> numDist(0, 9999);
    int number = numDist(gen);

    // Formata para sempre ter 4 dígitos
    char buf[10];
    snprintf(buf, sizeof(buf), " %04d", number);

    plate += buf;
    return plate;
}

inline std::string randomPlateLimited()
{
    // Letras permitidas para o prefixo
    static const std::vector<char> letters = {'A', 'C', 'E', 'P'};
    // Números permitidos para os dígitos
    static const std::vector<char> numbers = {'3', '4', '6', '7', '8'};

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distLetters(0, letters.size() - 1);
    std::uniform_int_distribution<> distNumbers(0, numbers.size() - 1);

    std::string plate;

    // 3 letras
    for (int i = 0; i < 3; ++i)
        plate += letters[distLetters(gen)];

    plate += " "; // espaço antes dos números

    // 4 números
    for (int i = 0; i < 4; ++i)
        plate += numbers[distNumbers(gen)];

    return plate;
}

inline std::string randomVIN()
{
    static const std::string allowedChars = "0123456789ABCDEFGHJKLMNPRSTUVWXYZ";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, allowedChars.size() - 1);

    std::string vin;
    vin.reserve(17);

    // WMI brasileiro (opcional)
    std::string wmi = "9BD"; // FIAT
    vin += wmi;

    // restante até 17 chars
    for (int i = 0; i < 14; i++) {
        vin += allowedChars[dis(gen)];
    }

    return vin;
}

inline std::string randomRENAVAM()
{
    // Gera 10 dígitos aleatórios (0-9)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    int nums[10];
    for (int i = 0; i < 10; i++)
        nums[i] = dist(gen);

    // Cálculo do dígito verificador (DV)
    // Peso oficial: 3 2 9 8 7 6 5 4 3 2
    static const int pesos[10] = {3,2,9,8,7,6,5,4,3,2};

    int soma = 0;
    for (int i = 0; i < 10; i++)
        soma += nums[i] * pesos[i];

    int dv = (soma * 10) % 11;
    if (dv == 10) dv = 0;

    // Monta a string final
    std::string renavam;
    renavam.reserve(11);
    for (int i = 0; i < 10; i++)
        renavam += std::to_string(nums[i]);
    renavam += std::to_string(dv);

    return renavam;
}

inline float CVectorDistance(const CVector& a, const CVector& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}