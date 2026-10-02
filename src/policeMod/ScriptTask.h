#pragma once

#include "../pch.h"

#define SCRIPT_KEEP_GOING 0
#define SCRIPT_SUCCESS 1
#define SCRIPT_CANCEL 2

class ScriptTask
{
  private:
    int timePassedToTimeout = 0;
    int timeout = -1;

  public:
    std::string name;
    int scriptStatus = SCRIPT_KEEP_GOING;
    int totalTimeElapsed = 0;
    bool deleteOnComplete = true;
    std::function<void()> onBegin;
    std::function<int()> onExecute;
    std::function<void()> onComplete;
    std::function<void()> onCancel;

    ScriptTask(std::string name);

    void Start();
    void SetStartAgainIfTimeout(int timeOut);

    static void MakeVehicleLeave(int vehicleRef);

    static void CreateTest();
};