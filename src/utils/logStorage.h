#pragma once

#include "mod/iaml.h"
#include "mod/logger.h"

#include <stdio.h>
#include <string>
#include <unordered_set>
#include <vector>

static constexpr size_t MAX_FIRST_MESSAGES = 200;
static constexpr size_t MAX_LAST_MESSAGES = 1000;

static std::vector<std::string> g_firstMessages;
static std::vector<std::string> g_lastMessages;
static size_t g_messageIndex = 0;

inline void OnLoggerMessage(eLogPrio prio, const char *msg) {
  if (!msg)
    return;

  if (g_firstMessages.size() < MAX_FIRST_MESSAGES)
    g_firstMessages.emplace_back(msg);

  if (g_lastMessages.size() < MAX_LAST_MESSAGES) {
    g_lastMessages.emplace_back(msg);
    return;
  }

  g_lastMessages[g_messageIndex] = msg;
  g_messageIndex = (g_messageIndex + 1) % MAX_LAST_MESSAGES;
}

inline std::string GetCrashFilePath() {
  std::string dataRootPath = aml->GetAndroidDataRootPath();

  return dataRootPath + "/mods/data/policeModSZK/lastcrash.log";
}

inline void SaveToFile() {
  auto path = GetCrashFilePath();

  FILE *file = fopen(path.c_str(), "w");

  if (!file)
    return;

  fprintf(file, "========== POLICE MOD CRASH LOG ==========\n\n");

  std::unordered_set<std::string> savedMessages;

  // Primeiras mensagens
  for (const auto &message : g_firstMessages) {
    if (savedMessages.insert(message).second)
      fprintf(file, "%s\n", message.c_str());
  }

  // Últimas mensagens
  if (g_lastMessages.size() < MAX_LAST_MESSAGES) {
    for (const auto &message : g_lastMessages) {
      if (savedMessages.insert(message).second)
        fprintf(file, "%s\n", message.c_str());
    }
  } else {
    for (size_t i = 0; i < MAX_LAST_MESSAGES; i++) {
      size_t index = (g_messageIndex + i) % MAX_LAST_MESSAGES;
      const auto &message = g_lastMessages[index];

      if (savedMessages.insert(message).second)
        fprintf(file, "%s\n", message.c_str());
    }
  }

  fclose(file);
}

inline void InitLogStorage() {
  logger->SetMessageCB(OnLoggerMessage);
  logger->Info("LogStorage initialized");
}