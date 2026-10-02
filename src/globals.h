#pragma once

#include "aml-psdk/game_sa/plugin.h"
#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include "policeMod/ModData.h"
#include "policeMod/ModLogger.h"
#include "utils/eventListener.h"

#include "include/menu.h"
inline IMenuSZK* menuSZK = nullptr;

#include "logUtils.h"

#include "include/isautils.h"
// inline ISAUtils *sautils = nullptr;

inline void LOGE(const char* format, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    logger->Error("%s", buffer);

    menuSZK->LogOnScreen(std::string(buffer), ScreenLogType::Error);
}

inline ModData* modData = new ModData("policeModSZK");

inline ITexture* textureBlip = nullptr;
inline ITexture* textureCircle = nullptr;
inline ITexture* textureBigCircle = nullptr;
inline ITexture* texturePoliceDP = nullptr;

inline int g_deltaTime = 0;

inline bool g_playerReady = false;

inline CVector* g_playerPosition = new CVector(0, 0, 0);
inline CVector2D g_defaultMenuPosition = CVector2D(2400 / 2.0f, 1080 / 2.0f);
inline int g_lastPlayerVehicle = -1;

inline EventListener<int>* g_onPedEnterVehicle = new EventListener<int>();
inline EventListener<int>* g_onPedLeaveVehicle = new EventListener<int>();
inline EventListener<int>* g_onVehicleDestroy = new EventListener<int>();

inline std::vector<int> g_vehiclesToDestroy;
inline std::vector<int> g_pedsToDestroy;

inline std::vector<int> g_criminalSkins = { 20, 18, 28, 66, 108 };

inline std::vector<int> g_stolenVehicleIds = { 405, 414, 440, 456, 461, 522, 482, 496, 499, 507 };

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

#define NO_PED_FOUND -1

#define COLOR_CRIMINAL CRGBA(255, 0, 0)
#define COLOR_YELLOW CRGBA(255, 255, 0)
#define COLOR_POLICE CRGBA(0, 150, 255)

inline std::string GetTranslatedText(std::string key)
{
    return menuSZK->GetLocalizationText(key);
}

inline std::string TT(std::string key)
{
    return GetTranslatedText(key);
}

template <typename... Args> inline std::string TT(const std::string& key, Args&&... args)
{
    std::string text = GetTranslatedText(key);

    std::vector<std::string> values = { std::to_string(std::forward<Args>(args))... };

    for (size_t i = 0; i < values.size(); i++)
    {
        const std::string placeholder = "{" + std::to_string(i) + "}";

        size_t position = 0;

        while ((position = text.find(placeholder, position)) != std::string::npos)
        {
            text.replace(position, placeholder.size(), values[i]);
            position += values[i].size();
        }
    }

    return text;
}

inline bool blockInput = false;

inline IWindow* CreatePM_Window(std::string title, std::string subtitle, float width, CVector2D position)
{
    auto window = menuSZK->CreateWindow(position.x, position.y, width, title, subtitle);
    window->windowColor = CRGBA(0, 150, 255);

    return window;
}

inline IWindow* CreatePM_Window(std::string title, std::string subtitle, float width = 800.0f)
{
    return CreatePM_Window(title, subtitle, width, g_defaultMenuPosition);
}