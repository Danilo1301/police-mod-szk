#pragma once

#include "mod/logger.h"
#include <string>
#include <vector>

// class LogHelper
// {
// public:
//     static void Initialize(const std::string& modName);
//     static void SetFrameOperation(const std::string& description);
//     static void CreateLogFile();

// private:
//     static void OnLoggerMessage(eLogPrio prio, const char* msg);
//     static void RemoveOldCrashLogs();

//     static constexpr size_t MAX_FIRST_MESSAGES = 200;
//     static constexpr size_t MAX_MESSAGES = 1000;
//     static constexpr size_t MAX_CRASH_LOGS = 3;

//     static std::string _modName;
//     static std::string _logsFolder;

//     static std::vector<std::string> _firstMessages;
//     static std::vector<std::string> _messages;
//     static size_t _messageIndex;
// };