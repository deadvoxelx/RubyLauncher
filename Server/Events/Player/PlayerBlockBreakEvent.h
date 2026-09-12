#pragma once

class ServerPlayer;
#include "Common/EventSystem/CactusEvent.h"
#include "Lua/LuaStructs.h"

struct PlayerBlockBreakEvent final : public CancellableCactusEvent {
    ServerPlayer* player;
    LuaBlock block;

    /* CactusModLoader [IMPL-AT] (Minecraft.Client/Player/ServerPlayerGameMode.cpp) */
    PlayerBlockBreakEvent(ServerPlayer* player, int x, int y, int z, int blockId, int blockAux) : player(player), block{LuaVec3(x,y,z), blockId, blockAux} {
        eventName = "PlayerBlockBreakEvent";
    }
};
