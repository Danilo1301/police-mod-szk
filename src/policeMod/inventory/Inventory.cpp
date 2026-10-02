#include "Inventory.h"

#include "InventoryItemManager.h"

Inventory::Inventory()
{

}

Inventory::~Inventory()
{

}

void Inventory::AddItem(const std::string& id, int amount)
{
    auto it = InventoryItemManager::itemDefinitions.find(id);
    if (it == InventoryItemManager::itemDefinitions.end())
        return; // id inválido

    auto def = it->second;

    // Tenta achar item existente
    for (auto& item : items)
    {
        if (item.GetDefinition() == def)
        {
            item.AddQuantity(amount);
            return;
        }
    }

    // Se não encontrou, adiciona um novo
    items.emplace_back(def, amount);
}

void Inventory::RemoveItem(const std::string& id)
{
    auto it = InventoryItemManager::itemDefinitions.find(id);
    if (it == InventoryItemManager::itemDefinitions.end())
        return; // id inválido

    auto def = it->second;

    // Remove qualquer item com essa definição
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const InventoryItem& item) {
                return item.GetDefinition() == def;
            }),
        items.end());
}