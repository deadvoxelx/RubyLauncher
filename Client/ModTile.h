#pragma once

#include "Tile.h"

#include <string>

struct ModTileTraits
{
    Material *material = nullptr;
    const Tile::SoundType *sound = nullptr;
    std::wstring textureIconName;
    std::wstring placeholderIconName;
    float hardness = 1.5f;
    float resistance = -1.0f;
    int craftingCategory = 0;
    int itemMaterial = 0;
};

class ModTile : public Tile {
public:
    ModTile(int id, const ModTileTraits& traits) : Tile(id, traits.material, true)
    {
        setIconName(traits.textureIconName.empty() ? traits.placeholderIconName : traits.textureIconName);
        setDestroyTime(traits.hardness);

        if (traits.resistance >= 0.0f) {
            setExplodeable(traits.resistance);
        }

        if (traits.sound != nullptr) {
            setSoundType(traits.sound);
        }

        setBaseItemTypeAndMaterial(traits.craftingCategory, traits.itemMaterial);
    }
};
