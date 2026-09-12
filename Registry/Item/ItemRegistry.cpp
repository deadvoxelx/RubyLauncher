#include "ItemRegistry.h"

#include "Item.h"

#include "Client/Rendering/ModTextureAtlas.h"
#include "ItemFactory.h"
#include "ModItem.h"
#include "Registry/IDs.h"

std::vector<std::wstring> ItemRegistry::langList(2000);
int ItemRegistry::itemNameIdMax = 1955;
int ItemRegistry::itemIdMax = 421;

int ItemRegistry::nextItemNameId() {
    itemNameIdMax += 1;
    return itemNameIdMax;
}

int ItemRegistry::nextItemId() {
    itemIdMax += 1;
    return itemIdMax;
}

int ItemRegistry::registerItem(const std::wstring& path, const std::string& id, const std::string& name, std::string modId, const ItemDefinition& def, const std::string& texturePath) {
    int nameId = nextItemNameId();
    int itemId = nextItemId();

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

            Item::items[itemId] = (ItemFactory::create(def, itemId - 256))
            ->setIconName(wname)
            ->handEquipped()
            ->setDescriptionId(nameId)
            ->setUseDescriptionId(IDS_DESC_STICK);
            IDMapping::get()->add(modId,id,false,itemId);

        }else {
            Item::items[itemId] = (new ModItem(itemId))->setIconName(L"stick")->handEquipped()->setDescriptionId(nameId)->setUseDescriptionId(IDS_DESC_STICK);
        }
    }


    langList[nameId] = wname;
    return itemId;
}

void ItemRegistry::changeLang(StringTable& m_stringTable) {
    // So c++ just deprecated wstring_convert with no alternative 🥀 either way we could just have used wstring from the start instead of storing langList as a list of strings
    //CML R: yeah but i HATE wstring ewwwww gross
    (void)m_stringTable;
}