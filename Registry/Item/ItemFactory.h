#pragma once

#include "Item.h"
#include "ArmorItem.h"
#include "FoodItem.h"
#include "HoeItem.h"
#include "WeaponItem.h"
#include "PickaxeItem.h"
#include "HatchetItem.h"
#include "ShovelItem.h"

#include <string>

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
    Helmet,
    Chestplate,
    Leggings,
    Boots,
};

enum EItemTier {
    ItemTier_Wood,
    ItemTier_Stone,
    ItemTier_Iron,
    ItemTier_Diamond,
    ItemTier_Gold,
    ItemTier_Nethanium,
    ItemTier_Endorium,
    ItemTier_Zanite,
    ItemTier_Gravitite,
    ItemTier_Vampire,
    ItemTier_Valkyrie,
    ItemTier_Aphalaf,
    ItemTier_Nusa,
};

enum EArmorMaterial {
    ArmorMaterial_Cloth,
    ArmorMaterial_Chain,
    ArmorMaterial_Iron,
    ArmorMaterial_Gold,
    ArmorMaterial_Diamond,
    ArmorMaterial_Nethanium,
    ArmorMaterial_Endorium,
    ArmorMaterial_Zanite,
    ArmorMaterial_Gravitite,
};

inline const ArmorItem::ArmorMaterial* armorMaterialFor(EArmorMaterial material) {
    switch (material) {
        case ArmorMaterial_Cloth:     return ArmorItem::ArmorMaterial::CLOTH;
        case ArmorMaterial_Chain:     return ArmorItem::ArmorMaterial::CHAIN;
        case ArmorMaterial_Gold:      return ArmorItem::ArmorMaterial::GOLD;
        case ArmorMaterial_Diamond:   return ArmorItem::ArmorMaterial::DIAMOND;
        case ArmorMaterial_Nethanium: return ArmorItem::ArmorMaterial::NETHANIUM;
        case ArmorMaterial_Endorium:  return ArmorItem::ArmorMaterial::ENDORIUM;
        case ArmorMaterial_Zanite:    return ArmorItem::ArmorMaterial::ZANITE;
        case ArmorMaterial_Gravitite: return ArmorItem::ArmorMaterial::GRAVITITE;
        case ArmorMaterial_Iron:
        default:                      return ArmorItem::ArmorMaterial::IRON;
    }
}

inline int vanillaArmorModelIndex(EArmorMaterial material) {
    switch (material) {
        case ArmorMaterial_Cloth:     return 0;
        case ArmorMaterial_Chain:     return 1;
        case ArmorMaterial_Diamond:   return 3;
        case ArmorMaterial_Gold:      return 4;
        case ArmorMaterial_Nethanium: return 5;
        case ArmorMaterial_Endorium:  return 6;
        case ArmorMaterial_Zanite:    return 7;
        case ArmorMaterial_Gravitite: return 8;
        case ArmorMaterial_Iron:
        default:                      return 2;
    }
}

inline int armorSlotFor(EBaseItem type) {
    switch (type) {
        case EBaseItem::Helmet:     return ArmorItem::SLOT_HEAD;
        case EBaseItem::Chestplate: return ArmorItem::SLOT_TORSO;
        case EBaseItem::Leggings:   return ArmorItem::SLOT_LEGS;
        case EBaseItem::Boots:      return ArmorItem::SLOT_FEET;
        default:                    return -1;
    }
}

inline const Item::Tier* tierFor(EItemTier tier) {
    switch (tier) {
        case ItemTier_Stone:     return Item::Tier::STONE;
        case ItemTier_Iron:      return Item::Tier::IRON;
        case ItemTier_Diamond:   return Item::Tier::DIAMOND;
        case ItemTier_Gold:      return Item::Tier::GOLD;
        case ItemTier_Nethanium: return Item::Tier::NETHANIUM;
        case ItemTier_Endorium:  return Item::Tier::ENDORIUM;
        case ItemTier_Zanite:    return Item::Tier::ZANITE;
        case ItemTier_Gravitite: return Item::Tier::GRAVITITE;
        case ItemTier_Vampire:   return Item::Tier::VAMPIRE;
        case ItemTier_Valkyrie:  return Item::Tier::VALKYRIE;
        case ItemTier_Aphalaf:   return Item::Tier::APHALAF;
        case ItemTier_Nusa:      return Item::Tier::NUSA;
        case ItemTier_Wood:
        default:                 return Item::Tier::WOOD;
    }
}

struct ItemDefinition {
    EBaseItem type;

    // Food Items
    int nutrition = 0;
    float saturationMod = 0.0;
    bool isMeat = false;

    // Tool Items
    const Item::Tier* tier = Item::Tier::WOOD;

    // Armor Items
    EArmorMaterial armorMaterial = ArmorMaterial_Iron;
    std::string armorSet = "";
    int armorModelIndex = -1;

    ItemDefinition(sol::table items) {
        type = items["base"].get<EBaseItem>();
        sol::optional<int> n = items["nutrition"];
        sol::optional<float> s = items["saturationMod"];
        sol::optional<bool> meat = items["isMeat"];
        sol::optional<Item::Tier*> t = items["tier"];
        sol::optional<std::string> armor = items["armorSet"];

        if (n) nutrition = n.value();
        if (s) saturationMod = s.value();
        if (meat) isMeat = meat.value();
        if (armor) armorSet = armor.value();

        if (items["armorMaterial"].is<int>()) {
            armorMaterial = items["armorMaterial"].get<EArmorMaterial>();
        }

        if (t) {
            tier = t.value();
        } else if (items["tier"].is<int>()) {
            tier = tierFor(items["tier"].get<EItemTier>());
        }

        if (armorModelIndex < 0) armorModelIndex = vanillaArmorModelIndex(armorMaterial);
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
            case EBaseItem::Helmet:
            case EBaseItem::Chestplate:
            case EBaseItem::Leggings:
            case EBaseItem::Boots:
                return (new ArmorItem(id, armorMaterialFor(def.armorMaterial), def.armorModelIndex, armorSlotFor(def.type)));
            case EBaseItem::Food:
                return (new FoodItem(id,def.nutrition,def.saturationMod,def.isMeat));
            case EBaseItem::Default:
            default:
                return (new ModItem(id));
        }
    }
};