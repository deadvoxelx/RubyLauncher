#pragma once

class ServerPlayer;
#include "Common/EventSystem/CactusEvent.h"

struct PlayerFlightEndedEvent final : public CactusEvent {
    ServerPlayer* player;

    /* CactusModLoader [IMPL-AT] (Minecraft.Client/Network/PendingConnection.cpp) */
    PlayerFlightEndedEvent(ServerPlayer* player) : player(player) {
        eventName = "FlightEndedEvent";
    }
};
