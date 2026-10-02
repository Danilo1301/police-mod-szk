#pragma once

#include "../../pch.h"

#include "ItemDefinition.h"

class InventoryItem
{
  public:
    InventoryItem(std::shared_ptr<ItemDefinition> definition, int amount = 1) : definition(definition), amount(amount)
    {
    }

    std::shared_ptr<ItemDefinition> GetDefinition() const { return definition; }
    int GetAmount() const { return amount; }

    void AddQuantity(int qtd) { amount += qtd; }
    void RemoveQuantity(int qtd) { amount = std::max(0, amount - qtd); }

  private:
    std::shared_ptr<ItemDefinition> definition;
    int amount = 1;
};