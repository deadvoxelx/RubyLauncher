#pragma once

#include "Common/EventSystem/RubyEvent.h"

struct ItemInteractEntityEvent final : public RubyEvent {
    ItemInstance* item;
    Mob* mob;

    /* Ruby Launcher [IMPL-AT] (Minecraft.World/Item/Item.cpp) */
    ItemInteractEntityEvent(ItemInstance item, Mob &mob) : item(&item), mob(&mob) {
        eventName = "ItemInteractEntity";
    }
};
