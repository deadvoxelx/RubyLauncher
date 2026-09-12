#pragma once

#include <vector>
#include <string>

#include "StringTable.h"

struct ItemDefinition;

class ItemRegistry {
    public:
        static int registerItem(const std::wstring& path, const std::string& id, const std::string& name, std::string modId, const ItemDefinition& def, const std::string& texturePath = "");
        static void changeLang(StringTable& m_stringTable);

        static int nextItemNameId();
        static int nextItemId();
        static std::vector<std::wstring> langList; //keep these 3 public for block registry

    private:
        static int itemNameIdMax;
        static int itemIdMax;
};