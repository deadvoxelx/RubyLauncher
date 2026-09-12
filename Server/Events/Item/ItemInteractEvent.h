#pragma once

class ServerLevel;
#include "ServerPlayer.h"

#include "Common/EventSystem/CactusEvent.h"

struct ItemInteractEvent final : public CactusEvent {
    ItemInstance* item;
    ServerLevel* level;
    ServerPlayer* player;


    /* CactusModLoader [IMPL-AT] (Minecraft.World/Item/Item.cpp) */
    ItemInteractEvent(ItemInstance* item, ServerLevel* level, ServerPlayer* player) : item(item), level(level), player(player) {
        eventName = "PlayerItemInteractEvent";
    }
};
