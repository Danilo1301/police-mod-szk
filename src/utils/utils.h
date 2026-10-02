#pragma once

#include "../globals.h"

#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include <aml-psdk/game_sa/engine/Radar.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>

inline double distanceBetweenPoints(CVector point1, CVector point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;
    double dz = point1.z - point2.z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

inline double distanceBetweenPoints2D(CVector2D point1, CVector2D point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;

    return sqrt(dx * dx + dy * dy);
}

inline std::string VectorToString(const CVector& vec, int _precision = 2)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(_precision);
    oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return oss.str();
}

inline int getRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do { x = rand(); } while (x >= RAND_MAX - remainder);
    return min + x % n;
}

inline bool calculateProbability(float chance)
{
    int i = getRandomNumber(0, 99);
    return i < (int)(chance * 100.0f);
}

inline void DrawTextureOnWorld(ITexture* texture, CVector worldPosition, CRGBA color, CVector2D size)
{
    constexpr float drawDistance = 20.0f;
    constexpr float fadeDistance = 5.0f;
    constexpr float minScale = 0.35f;

    CVector playerPos = *g_playerPosition;

    float distance = distanceBetweenPoints(playerPos, worldPosition);

    if (distance > drawDistance) return;

    float t = distance / drawDistance;
    float scale = 1.0f - (t * (1.0f - minScale));

    size.x *= scale;
    size.y *= scale;

    if (distance > drawDistance - fadeDistance)
    {
        float fade = (drawDistance - distance) / fadeDistance;
        color.a = (unsigned char)(color.a * fade);
    }

    CVector2D screenPosition = menuSZK->ConvertWorldToScreenCoords(worldPosition, true);

    screenPosition.x -= size.x / 2;
    screenPosition.y -= size.y / 2;

    menuSZK->DrawTexture(texture, screenPosition, size, color);
}

inline CVector2D WorldToRadarPoint(CVector worldPosition)
{
    CVector2D radarPosition;

    CRadar::TransformRealWorldPointToRadarSpace(radarPosition, CVector(worldPosition.x, worldPosition.y, 0.0f));

    return radarPosition;
}

inline bool file_exists(const std::string& path)
{
    std::ifstream f(path.c_str());
    return f.good();
}

inline std::string randomPlate()
{
    // Letras permitidas (pode adicionar/remover)
    static const std::vector<std::string> prefixes = { "EGC",
        "BRA",
        "MNT",
        "CPU",
        "LVR",
        "SJP",
        "FGT",
        "QXZ",
        "HJK",
        "VPE",
        "NDO",
        "RSC",
        "YAB",
        "ZGM",
        "PQL",
        "TJS",
        "WFM",
        "LXN",
        "CBF",
        "GVN" };

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
    static const std::vector<char> letters = { 'A', 'C', 'E', 'P' };
    // Números permitidos para os dígitos
    static const std::vector<char> numbers = { '3', '4', '6', '7', '8' };

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distLetters(0, letters.size() - 1);
    std::uniform_int_distribution<> distNumbers(0, numbers.size() - 1);

    std::string plate;

    // 3 letras
    for (int i = 0; i < 3; ++i) plate += letters[distLetters(gen)];

    plate += " "; // espaço antes dos números

    // 4 números
    for (int i = 0; i < 4; ++i) plate += numbers[distNumbers(gen)];

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
    for (int i = 0; i < 14; i++) { vin += allowedChars[dis(gen)]; }

    return vin;
}

inline std::string randomRENAVAM()
{
    // Gera 10 dígitos aleatórios (0-9)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    int nums[10];
    for (int i = 0; i < 10; i++) nums[i] = dist(gen);

    // Cálculo do dígito verificador (DV)
    // Peso oficial: 3 2 9 8 7 6 5 4 3 2
    static const int pesos[10] = { 3, 2, 9, 8, 7, 6, 5, 4, 3, 2 };

    int soma = 0;
    for (int i = 0; i < 10; i++) soma += nums[i] * pesos[i];

    int dv = (soma * 10) % 11;
    if (dv == 10) dv = 0;

    // Monta a string final
    std::string renavam;
    renavam.reserve(11);
    for (int i = 0; i < 10; i++) renavam += std::to_string(nums[i]);
    renavam += std::to_string(dv);

    return renavam;
}

inline int GetRandomCriminalSkin()
{
    if (g_criminalSkins.empty()) return -1;

    int index = getRandomNumber(0, static_cast<int>(g_criminalSkins.size()) - 1);
    return g_criminalSkins[index];
}

inline float CVectorDistance(const CVector& a, const CVector& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

inline void CreateFullPath(const std::string& path)
{
    std::error_code error;
    std::filesystem::create_directories(path, error);

    if (error) { logger->Error("Failed to create folder: %s (%s)", path.c_str(), error.message().c_str()); }
}