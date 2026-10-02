#include "ModelLoader.h"

#include "opcodeCaller/CleoFunctions.h"
#include <string>

std::vector<int> ModelLoader::modelsToLoad;
std::function<void()> ModelLoader::onLoadAllCallback;

void ModelLoader::AddModelToLoad(int modelId)
{
    if (!HAS_MODEL_LOADED(modelId)) { modelsToLoad.push_back(modelId); }
}

void ModelLoader::LoadAll(std::function<void()> callback)
{
    onLoadAllCallback = callback;
    LoadNextModel();
}

void ModelLoader::LoadNextModel()
{
    if (modelsToLoad.empty())
    {
        if (onLoadAllCallback) onLoadAllCallback();
        return;
    }

    int modelId = modelsToLoad.front();

    if (HAS_MODEL_LOADED(modelId))
    {
        ModelLoaded(modelId);
        return;
    }

    if (IsSpecialModel(modelId)) { logger->Error("You tried to load a SPECIAL model %d and this will cause CRASH", modelId); }

    logger->Info("ModelLoader: Loading model %d", modelId);

    LOAD_MODEL(modelId);
    LOAD_REQUESTED_MODELS();

    if (HAS_MODEL_LOADED(modelId))
    {
        ModelLoaded(modelId);
        return;
    }

    CleoFunctions::AddWaitForFunction(
        "modelloader_load", [modelId]() { return HAS_MODEL_LOADED(modelId); }, [modelId]() { ModelLoaded(modelId); });
}

void ModelLoader::ModelLoaded(int modelId)
{
    // remove o primeiro da fila
    if (!modelsToLoad.empty() && modelsToLoad.front() == modelId) modelsToLoad.erase(modelsToLoad.begin());

    // carrega o próximo
    LoadNextModel();
}

bool ModelLoader::IsSpecialModel(int id)
{
    // lista simplificada
    static std::vector<int> specialModels = { 265, 266, 267, 274, 275, 276, 277, 278, 279, 288 };

    return std::find(specialModels.begin(), specialModels.end(), id) != specialModels.end();
}