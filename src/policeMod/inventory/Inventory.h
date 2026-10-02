#pragma once

#include "../../pch.h"

#include "InventoryItem.h"
#include "ItemDefinition.h"

class Inventory
{
  public:
    Inventory();
    ~Inventory();

    void MarkAsInitialized() { hasInitialized = true; }
    bool HasInitialized() const { return hasInitialized; }

    void AddItem(const std::string &id, int amount = 1);
    void RemoveItem(const std::string &id);
    const std::vector<InventoryItem> &GetItems() const { return items; }

  private:
    bool hasInitialized = false;
    std::vector<InventoryItem> items;
};