#pragma once

#include "Item.h"
#include "FoodItem.h"
#include "HoeItem.h"
#include "WeaponItem.h"
#include "PickaxeItem.h"
#include "HatchetItem.h"
#include "ShovelItem.h"

#include "sol/sol.hpp"

#include "ModItem.h"

enum EBaseItem {
    Default,
    Food,
    Hoe,
    Weapon,
    Pickaxe,
    Hatchet,
    Shovel,
};

struct ItemDefinition {
    EBaseItem type;

    // Food Items
    int nutrition = 0;
    float saturationMod = 0.0;
    bool isMeat = false;

    // Tool Items
    const Item::Tier* tier = Item::Tier::WOOD;

    ItemDefinition(sol::table items) {
        type = items["base"].get<EBaseItem>();
        sol::optional<int> n = items["nutrition"];
        sol::optional<float> s = items["saturationMod"];
        sol::optional<bool> meat = items["isMeat"];
        sol::optional<Item::Tier*> t = items["tier"];

        if (n) nutrition = n.value();
        if (s) saturationMod = s.value();
        if (meat) isMeat = meat.value();
        if (t) tier = t.value();
    }
};

class ItemFactory {
public:
    static Item* create(const ItemDefinition& def, int id) {
        switch (def.type) {
            case EBaseItem::Shovel:
                return (new ShovelItem(id,def.tier));
            case EBaseItem::Hatchet:
                return (new HatchetItem(id,def.tier));
            case EBaseItem::Pickaxe:
                return (new PickaxeItem(id,def.tier));
            case EBaseItem::Weapon:
                return (new WeaponItem(id,def.tier));
            case EBaseItem::Hoe:
                return (new HoeItem(id,def.tier));
            case EBaseItem::Food:
                return (new FoodItem(id,def.nutrition,def.saturationMod,def.isMeat));
            case EBaseItem::Default:
            default:
                return (new ModItem(id));
        }
    }
};