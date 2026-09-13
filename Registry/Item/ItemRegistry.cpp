#include "ItemRegistry.h"

#include "Item.h"

#include "Common/ModPaths.h"
#include "Host/RubyLauncherHost.h"
#include "ItemFactory.h"
#include "Loader.h"
#include "ModItem.h"
#include "Registry/IDs.h"

std::map<int, std::wstring> ItemRegistry::langList;
int ItemRegistry::itemNameIdMax = 2444;
int ItemRegistry::itemIdMax = 812;

namespace
{
	bool isReservedItemId(int id)
	{
		return (id >= 256 && id < 512) || (id >= 2256 && id < 2268);
	}

	int useDescriptionFor(EBaseItem type)
	{
		switch (type)
		{
		case EBaseItem::Weapon:
			return IDS_DESC_SWORD;
		case EBaseItem::Pickaxe:
			return IDS_DESC_PICKAXE;
		case EBaseItem::Hatchet:
			return IDS_DESC_HATCHET;
		case EBaseItem::Shovel:
			return IDS_DESC_SHOVEL;
		case EBaseItem::Hoe:
			return IDS_DESC_HOE;
		case EBaseItem::Helmet:
			return IDS_DESC_HELMET_LEATHER;
		case EBaseItem::Chestplate:
			return IDS_DESC_CHESTPLATE_LEATHER;
		case EBaseItem::Leggings:
			return IDS_DESC_LEGGINGS_LEATHER;
		case EBaseItem::Boots:
			return IDS_DESC_BOOTS_LEATHER;
		case EBaseItem::Food:
		case EBaseItem::Default:
		default:
			return IDS_DESC_STICK;
		}
	}

	RubyCreativeGroup creativeGroupFor(EBaseItem type)
	{
		switch (type)
		{
		case EBaseItem::Food:
			return RubyCreativeGroup_Food;
		case EBaseItem::Weapon:
		case EBaseItem::Hoe:
		case EBaseItem::Pickaxe:
		case EBaseItem::Hatchet:
		case EBaseItem::Shovel:
		case EBaseItem::Helmet:
		case EBaseItem::Chestplate:
		case EBaseItem::Leggings:
		case EBaseItem::Boots:
			return RubyCreativeGroup_Tools;
		case EBaseItem::Default:
		default:
			return RubyCreativeGroup_Materials;
		}
	}

	int baseItemTypeFor(EBaseItem type)
	{
		switch (type)
		{
		case EBaseItem::Helmet:
			return Item::eBaseItemType_helmet;
		case EBaseItem::Chestplate:
			return Item::eBaseItemType_chestplate;
		case EBaseItem::Leggings:
			return Item::eBaseItemType_leggings;
		case EBaseItem::Boots:
			return Item::eBaseItemType_boots;
		default:
			return Item::eBaseItemType_undefined;
		}
	}

	int armorMaterialTypeFor(EArmorMaterial material)
	{
		switch (material)
		{
		case ArmorMaterial_Cloth:
			return Item::eMaterial_cloth;
		case ArmorMaterial_Chain:
			return Item::eMaterial_chain;
		case ArmorMaterial_Gold:
			return Item::eMaterial_gold;
		case ArmorMaterial_Diamond:
			return Item::eMaterial_diamond;
		case ArmorMaterial_Nethanium:
			return Item::eMaterial_nethanium;
		case ArmorMaterial_Endorium:
			return Item::eMaterial_endorium;
		case ArmorMaterial_Zanite:
			return Item::eMaterial_zanite;
		case ArmorMaterial_Gravitite:
			return Item::eMaterial_gravitite;
		case ArmorMaterial_Iron:
		default:
			return Item::eMaterial_iron;
		}
	}
}

int ItemRegistry::nextItemNameId() {
    itemNameIdMax += 1;
    return itemNameIdMax;
}

int ItemRegistry::nextItemId() {
    do {
        itemIdMax += 1;
    } while (itemIdMax < Item::ITEM_NUM_COUNT && (isReservedItemId(itemIdMax) || Item::items[itemIdMax] != nullptr));

    return itemIdMax;
}

int ItemRegistry::registerItem(const std::wstring& path, const std::string& id, const std::string& name, std::string modId, const ItemDefinition& def, const std::string& texturePath) {
    int nameId = nextItemNameId();
    int itemId = nextItemId();

    if (itemId >= Item::ITEM_NUM_COUNT) {
        Loader::_debugPrint("out of item ids, '" + id + "' was not registered");
        return -1;
    }

    std::wstring wname(name.begin(), name.end());

    const bool isArmor = armorSlotFor(def.type) >= 0;
    ItemDefinition resolved = def;

    if (isArmor)
    {
        resolved.armorModelIndex = RubyLoader::registerArmorSet(def.armorSet, RubyPaths::toNarrow(path), vanillaArmorModelIndex(def.armorMaterial));
    }

    Item *item = ItemFactory::create(resolved, itemId - 512);

    if (item == nullptr) {
        Loader::_debugPrint("could not create item '" + id + "'");
        return -1;
    }

    const std::wstring placeholderIconName = L"stick";
    std::wstring iconName = placeholderIconName;

    if (!texturePath.empty())
    {
        RubyModTexture request;
        request.iconName = rubyModTextureIconName(modId, id);
        request.filePath = RubyPaths::resolveModTexturePath(RubyPaths::toNarrow(path), texturePath);
        request.fallback = RubyPaths::toNarrow(placeholderIconName);
        request.block = false;

        RubyLoader::addModTexture(request);
        iconName = RubyPaths::toWide(request.iconName);
    }

    item->setIconName(iconName)->setDescriptionId(nameId)->setUseDescriptionId(useDescriptionFor(def.type));

    if (isArmor)
    {
        item->setBaseItemTypeAndMaterial(baseItemTypeFor(def.type), armorMaterialTypeFor(def.armorMaterial));
    }
    else
    {
        item->handEquipped();
    }

    langList[nameId] = wname;
    IDMapping::get()->add(modId, id, false, itemId);

    RubyCreative::addEntry(itemId, 0, creativeGroupFor(def.type));

    Loader::_debugPrint("registered item " + modId + ":" + id + " as id " + std::to_string(itemId) + (texturePath.empty() ? "" : ", texture '" + texturePath + "'"));

    return itemId;
}

void ItemRegistry::changeLang(StringTable& m_stringTable) {
    for (const auto& entry : langList) {
        m_stringTable.registerString(entry.first, entry.second);
    }
}
