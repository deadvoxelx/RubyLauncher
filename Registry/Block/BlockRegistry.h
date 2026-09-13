#pragma once

#include "sol/sol.hpp"

#include <string>

enum EBlockTool {
    BlockTool_None = 0,
    BlockTool_Pickaxe,
    BlockTool_Hatchet,
    BlockTool_Shovel,
    BlockTool_Hoe,
};

struct BlockDefinition {
    float hardness = 1.5f;
    float resistance = -1.0f;

    EBlockTool tool = BlockTool_Pickaxe;

    BlockDefinition() = default;
    BlockDefinition(sol::table table);
};

class BlockRegistry {
public:
    static int nextItemId();

    static int maxBlockCount();

    static int registerBlock(const std::wstring& path, const std::string& id, const std::string& name, std::string modId, const std::string& texturePath, const BlockDefinition& def);
};
