#pragma once

#include "ServerPlayer.h"

#include "Common/EventSystem/CactusEvent.h"

struct PlayerJoinEvent final : public CactusEvent {
    ServerPlayer* player;

    /* CactusModLoader [IMPL-AT] (Minecraft.Client/Network/PlayerList.cpp) */
    PlayerJoinEvent(ServerPlayer* player) : player(player) {
        eventName = "PlayerJoinEvent";
    }
};
