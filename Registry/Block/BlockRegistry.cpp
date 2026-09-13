#include "BlockRegistry.h"

#include "Item.h"
#include "Loader.h"
#include "Material.h"
#include "Registry/IDs.h"
#include "Tile.h"
#include "TileItem.h"

#include "Client/ModTile.h"
#include "Common/ModPaths.h"
#include "Host/RubyLauncherHost.h"
#include "Registry/Item/ItemRegistry.h"

#if defined(_WINDOWS64)
#include "Windows64Media/strings.h"
#else
#include "DurangoMedia/loc/strings.h"
#endif

namespace
{
	const int TILE_ID_MIN = 269;	// Dark Nustone Stairs; this has to be changed whenever i add new blocks to the base
	const int TILE_ID_MAX = 511;

	int tileIdMax = TILE_ID_MIN - 1;

	ModTileTraits traitsFor(const BlockDefinition &def)
	{
		ModTileTraits traits;

		switch (def.tool)
		{
		case BlockTool_None:
			traits.material = Material::dirt;
			traits.sound = Tile::SOUND_GRAVEL;
			traits.placeholderIconName = L"dirt";
			traits.craftingCategory = Item::eBaseItemType_structblock;
			traits.itemMaterial = Item::eMaterial_undefined;
			break;
		case BlockTool_Hatchet:
			traits.material = Material::wood;
			traits.sound = Tile::SOUND_WOOD;
			traits.placeholderIconName = L"planks_oak";
			traits.craftingCategory = Item::eBaseItemType_structwoodstuff;
			traits.itemMaterial = Item::eMaterial_wood;
			break;
		case BlockTool_Shovel:
			traits.material = Material::dirt;
			traits.sound = Tile::SOUND_SAND;
			traits.placeholderIconName = L"sand";
			traits.craftingCategory = Item::eBaseItemType_structblock;
			traits.itemMaterial = Item::eMaterial_sand;
			break;
		case BlockTool_Hoe:
			traits.material = Material::dirt;
			traits.sound = Tile::SOUND_GRASS;
			traits.placeholderIconName = L"dirt";
			traits.craftingCategory = Item::eBaseItemType_structblock;
			traits.itemMaterial = Item::eMaterial_undefined;
			break;
		case BlockTool_Pickaxe:
		default:
			traits.material = Material::stone;
			traits.sound = Tile::SOUND_STONE;
			traits.placeholderIconName = L"stone";
			traits.craftingCategory = Item::eBaseItemType_structblock;
			traits.itemMaterial = Item::eMaterial_stone;
			break;
		}

	traits.hardness = def.hardness;
		traits.resistance = def.resistance;

		return traits;
	}
}

BlockDefinition::BlockDefinition(sol::table table)
{
	sol::optional<float> hardnessValue = table["hardness"];
	if (hardnessValue) hardness = hardnessValue.value();

	sol::optional<float> resistanceValue = table["resistance"];
	if (resistanceValue) resistance = resistanceValue.value();

	sol::optional<EBlockTool> toolValue = table["tool"];
	if (toolValue) tool = toolValue.value();
}

int BlockRegistry::maxBlockCount() {
	return TILE_ID_MAX - TILE_ID_MIN + 1;
}

int BlockRegistry::nextItemId() {
	do {
		tileIdMax += 1;
	} while (tileIdMax <= TILE_ID_MAX && (Tile::tiles[tileIdMax] != nullptr || Item::items[tileIdMax] != nullptr));

	return (tileIdMax > TILE_ID_MAX) ? -1 : tileIdMax;
}

int BlockRegistry::registerBlock(const std::wstring& path, const std::string& id, const std::string& name, std::string modId, const std::string& texturePath, const BlockDefinition& def) {
	int nameId = ItemRegistry::nextItemNameId();
	int tileId = BlockRegistry::nextItemId();

	if (tileId == -1) {
		Loader::_debugPrint("out of block ids (" + std::to_string(TILE_ID_MIN) + "-" + std::to_string(TILE_ID_MAX) + "), '" + id + "' was not registered");
		return -1;
	}

	std::wstring wname(name.begin(), name.end());

	ModTileTraits traits = traitsFor(def);

	if (!texturePath.empty())
	{
		RubyModTexture request;
		request.iconName = rubyModTextureIconName(modId, id);
		request.filePath = RubyPaths::resolveModTexturePath(RubyPaths::toNarrow(path), texturePath);
		request.fallback = RubyPaths::toNarrow(traits.placeholderIconName);
		request.block = true;

		RubyLoader::addModTexture(request);
		traits.textureIconName = RubyPaths::toWide(request.iconName);
	}

	Tile *tile = new ModTile(tileId, traits);
	tile->setDescriptionId(nameId);

	Item::items[tileId] = (new TileItem(tileId - 512))
		->setIconName(traits.textureIconName.empty() ? traits.placeholderIconName : traits.textureIconName)
		->setDescriptionId(nameId)
		->setBaseItemTypeAndMaterial(traits.craftingCategory, traits.itemMaterial)
		->setUseDescriptionId(IDS_DESC_STONE);

	ItemRegistry::langList[nameId] = wname;
	IDMapping::get()->add(modId, id, true, tileId);

	RubyCreative::addEntry(tileId, 0, RubyCreativeGroup_BuildingBlocks);
	Loader::_debugPrint("registered block " + modId + ":" + id + " as tile " + std::to_string(tileId) + " (hardness " + std::to_string(def.hardness) + ")" + (texturePath.empty() ? "" : ", texture '" + texturePath + "'"));

	return tileId;
}
