#pragma once

class ServerPlayer;
#include "Common/EventSystem/CactusEvent.h"
#include "Lua/LuaStructs.h"

struct PlayerBlockPlaceEvent final : public CancellableCactusEvent {
    ServerPlayer* player;
    LuaBlock block;

    /* CactusModLoader [IMPL-AT] (Minecraft.Client/Network/PlayerConnection.cpp) */
    PlayerBlockPlaceEvent(ServerPlayer* player, int x, int y, int z, int blockId) : player(player), block{LuaVec3(x,y,z), blockId} {
        eventName = "PlayerBlockPlaceEvent";
    }
};