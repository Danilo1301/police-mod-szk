#pragma once

#include "../../pch.h"

#include "ItemDefinition.h"

class InventoryItemManager
{
  public:
    static std::unordered_map<std::string, std::shared_ptr<ItemDefinition>> itemDefinitions;

    static void Initialize();
    static void LoadItems();
};