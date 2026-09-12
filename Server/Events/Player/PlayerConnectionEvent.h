#pragma once

class ServerPlayer;
#include "Common/EventSystem/CactusEvent.h"

struct PlayerConnectionEvent final : public CactusEvent {
    ServerPlayer* player;

    /* CactusModLoader [IMPL-AT] (Minecraft.Client/Network/PendingConnection.cpp) */
    PlayerConnectionEvent(ServerPlayer* player) : player(player) {
        eventName = "PlayerConnectionEvent";
    }
};
