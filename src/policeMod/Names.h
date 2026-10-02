#pragma once

#include "../pch.h"

class Names
{
  public:
    static void Initialize();
    static std::vector<std::string> LoadNamesFromFile(std::string file);

    static std::string GetName(bool male);
};