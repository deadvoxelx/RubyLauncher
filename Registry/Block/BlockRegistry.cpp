#include "BlockRegistry.h"

#include <unordered_map>

#include "DurangoMedia/loc/strings.h"
#include "Tile.h"
#include "TileItem.h"
#include "Item.h"
#include "Material.h"

#include "Client/Rendering/ModTextureAtlas.h"
#include "Registry/Item/ItemRegistry.h"
#include "Client/ModTile.h"
#include "Registry/IDs.h"

int itemIdMax = 173;

int BlockRegistry::nextItemId() {
    itemIdMax += 1;
    if (itemIdMax == 255) {
        return -1;
    }
    return itemIdMax;
}


int BlockRegistry::registerBlock(const std::wstring& path, const std::string& id, const std::string& name, std::string modId, const std::string& texturePath) {
    int nameId = ItemRegistry::nextItemNameId();
    int itemId = BlockRegistry::nextItemId();

    if (itemId == -1) {
        return -1;
    }

    std::wstring wname(name.begin(), name.end());

    if (!texturePath.empty() && ModTextureAtlas::getInstance() != nullptr) {
        std::wstring wpath(texturePath.begin(), texturePath.end());

        BufferedImage* img = new BufferedImage(wpath, true, false, L"mods/"+path+L"/");

        if (img != nullptr) {
            int w = img->getWidth();
            int h = img->getHeight();

            std::vector<int> pixels(w*h);

            intArray wrapper(pixels.data(), w*h);

            img->getRGB(0,0,w,h,wrapper,0,w);

            ModTextureAtlas::getInstance()->registerTexture(wname, std::move(pixels), w, h);
            delete img;

            new ModTile(itemId, Material::dirt, wname);

            Tile::tiles[itemId]->setDescriptionId(nameId);
            Tile::tiles[itemId]->setUseDescriptionId(IDS_DESC_STICK);

            Item::items[itemId] = (new TileItem(itemId - 256))
                ->setIconName(wname)
                ->handEquipped()
                ->setDescriptionId(nameId)
                ->setUseDescriptionId(IDS_DESC_STICK);

            IDMapping::get()->add(modId,id,true,itemId);
        } else {
            new ModTile(itemId, Material::grass, L"dirt");
            Item::items[itemId] = (new TileItem(itemId-256))->setIconName(L"stick")->handEquipped()->setDescriptionId(nameId)->setUseDescriptionId(IDS_DESC_STICK);
        }
    }


    ItemRegistry::langList[nameId] = wname;
    return itemId;
}