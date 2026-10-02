#pragma once

#include "../../pch.h"

struct ItemDefinition
{
    std::string name;
    std::string description;
    int maxAmount = 1;
    bool isIllegal = false;
    double chance = 1.00;
    std::string flags = ""; // flag1, flag2, flag3
    bool isStolen = false;
    std::string amountFormat = "";

    ItemDefinition() {};
    ItemDefinition(const std::string &name, const std::string &description, int maxAmount = 1)
        : name(name), description(description), maxAmount(maxAmount)
    {
    }
};

inline bool ItemDefinitionContainFlag(ItemDefinition definition, std::string flag)
{
    // procura o flag dentro da string de flags
    return definition.flags.find(flag) != std::string::npos;
}

inline std::string FormatAmount(ItemDefinition def, int amount)
{
    // se não houver formato, usa "xN" normal
    if (def.amountFormat.empty() || def.amountFormat.find("~1~") == std::string::npos)
        return def.name + " x" + std::to_string(amount);

    std::string text = def.amountFormat;

    // substitui ~1~ pela quantidade real
    size_t pos = text.find("~1~");
    if (pos != std::string::npos)
        text.replace(pos, 3, std::to_string(amount));

    return def.name + " (" + text + ")";
}