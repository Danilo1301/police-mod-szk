#pragma once

#include "../pch.h"

struct IMessage
{
    std::string text;
    int time;
};

class BottomMessage
{
  public:
    static std::vector<IMessage> Messages;

    static void Initialize();

    static void Update();

    static void AddMessage(const std::string &message, int timeMs);
    static void SetMessage(const std::string &message, int timeMs);
    static void ClearMessages();
};