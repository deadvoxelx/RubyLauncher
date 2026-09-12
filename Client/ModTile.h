#pragma once

#include "StitchedTexture.h"
#include "Icon.h"
#include "Tile.h"

#include "Rendering/ModTextureAtlas.h"

class ModTile : public Tile {
    std::wstring m_modTextureName;

public:
    ModTile(int id, Material* material, const std::wstring& modTextureName) : Tile(id, material, true), m_modTextureName(modTextureName) {}

    Icon* getTexture(int face) override {
        if (!ModTextureAtlas::getInstance()) return nullptr;
        return ModTextureAtlas::getInstance()->getIcon(m_modTextureName);
    }
    Icon* getTexture(int face, int data) override {
        return getTexture(face);
    }
    Icon* getTexture(LevelSource *level, int x, int y, int z, int face) override {
        return getTexture(face);
    }
};
