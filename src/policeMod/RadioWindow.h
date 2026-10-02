#pragma once

#include "../pch.h"

struct RadioScreen
{
    std::string texturePath;
};

struct RadioScreenGroup
{
    std::vector<std::pair<std::string, RadioScreen>> screens;
    std::string backToGroupId; // grupo para o qual deve voltar
};

class RadioWindow
{
  public:
    static IContainer *MainContainer;
    static IContainer *ScreenContainer;

    static bool m_cancelServices;

    // Todos os grupos de telas
    static std::map<std::string, RadioScreenGroup> groups;

    // Grupo atualmente ativo
    static std::string currentGroupId;

    static int currentScreenIndex;

    static void Initialize();
    static void Toggle();

    // Adiciona um grupo (opcionalmente com link de "voltar")
    static void AddGroup(const std::string &groupId, const std::string &backTo = "")
    {
        RadioScreenGroup group;
        group.backToGroupId = backTo;
        groups[groupId] = group;
    }

    // Adiciona uma tela dentro de um grupo
    static void AddScreen(const std::string &groupId, const std::string &screenId, const std::string &textureName)
    {
        std::string textureSrc = GetModAssetPath("radio/screens/" + textureName);

        RadioScreen rs;
        rs.texturePath = textureSrc;

        auto &group = groups[groupId];
        group.screens.emplace_back(screenId, rs);
    }

    // Troca para outro grupo
    static void SwitchToGroup(const std::string &groupId)
    {
        if (groups.find(groupId) == groups.end())
            return;

        currentGroupId = groupId;
        currentScreenIndex = 0;

        // Aqui você poderia atualizar o conteúdo da UI (ex: mudar telas exibidas)

        UpdateScreenContainer();
    }

    // Volta para o grupo anterior
    static void Back()
    {
        if (currentGroupId.empty())
            return;

        auto it = groups.find(currentGroupId);
        if (it == groups.end())
            return;

        const std::string &backTo = it->second.backToGroupId;
        if (!backTo.empty())
        {
            SwitchToGroup(backTo);
        }
        else
        {
            Toggle();
        }
    }

    static void Select()
    {
        auto itGroup = groups.find(currentGroupId);
        if (itGroup == groups.end())
            return;

        auto &screens = itGroup->second.screens;
        if (screens.empty())
            return;

        if (currentScreenIndex < 0 || currentScreenIndex >= (int)screens.size())
            return;

        // Obtém a tela atual
        auto &screenPair = screens[currentScreenIndex];
        const std::string &screenId = screenPair.first;
        // RadioScreen& screen = screenPair.second;

        // Chama callback genérico pra notificar que foi selecionada
        OnSelect(screenId);
    }

    static void OnSelect(std::string id);

    // 👉 Vai para a próxima tela (Right)
    static void NextScreen()
    {
        auto it = groups.find(currentGroupId);
        if (it == groups.end())
            return;

        auto &screens = it->second.screens;
        if (screens.empty())
            return;

        currentScreenIndex = (currentScreenIndex + 1) % screens.size();

        UpdateScreenContainer();
    }

    // 👉 Vai para a tela anterior (Left)
    static void PrevScreen()
    {
        auto it = groups.find(currentGroupId);
        if (it == groups.end())
            return;

        auto &screens = it->second.screens;
        if (screens.empty())
            return;

        currentScreenIndex = (currentScreenIndex - 1 + (int)screens.size()) % screens.size();

        UpdateScreenContainer();
    }

    static RadioScreen *GetCurrentScreen()
    {
        auto it = groups.find(currentGroupId);
        if (it == groups.end())
            return nullptr;

        auto &screens = it->second.screens;
        if (screens.empty())
            return nullptr;

        if (currentScreenIndex < 0 || currentScreenIndex >= (int)screens.size())
            return nullptr;

        return &screens[currentScreenIndex].second;
    }

    static void UpdateScreenContainer()
    {
        if (!ScreenContainer)
            return;

        auto *screen = RadioWindow::GetCurrentScreen();
        if (screen)
        {
            ScreenContainer->style.backgroundImage = screen->texturePath;
        }
    }

    static void ToggleRadioAnim(bool state);
    static void ToggleRadioAnimOff(int timeMs);
};