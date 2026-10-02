#pragma once

#include "../pch.h"

class ModelLoader
{
  private:
    static std::vector<int> modelsToLoad;
    static std::function<void()> onLoadAllCallback;

  public:
    static void AddModelToLoad(int modelId);
    static void LoadAll(std::function<void()> callback);

  private:
    static void LoadNextModel();
    static void ModelLoaded(int modelId);

  public:
    static bool IsSpecialModel(int id);
};