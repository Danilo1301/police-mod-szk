#pragma once
#include <string>
#include <vector>

struct WeaponInfo
{
    int weaponId;      // ID da arma (ex: 22 = pistola)
    int modelId;       // ID do modelo 3D da arma
    std::string name;  // Nome da arma
};

inline std::vector<WeaponInfo> g_weaponList = {
    {22, 346, "Pistol 9mm"},         // weaponId, modelId, name
    {23, 347, "Silenced Pistol"},
    {24, 348, "Desert Eagle"},
    {25, 349, "Shotgun"},
    {26, 350, "Sawnoff Shotgun"},
    {27, 351, "Combat Shotgun"},
    {28, 352, "Micro Uzi"},
    {29, 353, "MP5"},
    {30, 355, "AK-47"},
    {31, 356, "M4"},
    {32, 372, "Tec-9"},
    {33, 357, "Country Rifle"},
    {34, 358, "Sniper Rifle"},
    {35, 359, "RPG"},
    {36, 360, "HS Rocket"},
    {37, 361, "Flamethrower"},
    {38, 362, "Minigun"}
};