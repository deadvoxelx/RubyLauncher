#pragma once

class ServerPlayer;
#include "Common/EventSystem/CactusEvent.h"

struct PlayerFlightStartedEvent final : public CactusEvent {
    ServerPlayer* player;

    /* CactusModLoader [IMPL-AT] (Minecraft.Client/Network/PendingConnection.cpp) */
    PlayerFlightStartedEvent(ServerPlayer* player) : player(player) {
        eventName = "FlightStartedEvent";
    }
};
