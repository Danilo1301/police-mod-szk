#include "Names.h"

std::vector<std::string> g_maleNames;
std::vector<std::string> g_femaleNames;

void Names::Initialize()
{
    g_maleNames = LoadNamesFromFile(modData->GetFile("/data/names/male.txt"));
    g_femaleNames = LoadNamesFromFile(modData->GetFile("/data/names/female.txt"));
}

std::vector<std::string> Names::LoadNamesFromFile(const std::string path)
{
    std::vector<std::string> result;

    std::ifstream file(path);
    if (!file.is_open())
    {
        //printf("[Names] Erro ao abrir arquivo: %s\n", path.c_str());
        return result;
    }

    std::string line;

    while (std::getline(file, line))
    {
        // Remove \r e \n
        line.erase(std::remove_if(line.begin(), line.end(),
            [](char c){ return c == '\r' || c == '\n'; }), line.end());

        if (line.empty())
            continue;

        result.push_back(line);
    }

    return result;
}

std::string Names::GetName(bool male)
{
    if (male)
    {
        if (!g_maleNames.empty())
        {
            int index = rand() % g_maleNames.size();
            return g_maleNames[index];
        }
        return "Nome Masculino";
    } 

    if (!g_femaleNames.empty())
    {
        int index = rand() % g_femaleNames.size();
        return g_femaleNames[index];
    }
    return "Nome Feminino";
}