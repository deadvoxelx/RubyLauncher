-- Server scripts run when a world is hosted; this is where event listeners are registered.
function main()
    registerEvent("PlayerJoinEvent", function(event)
        local player = event.player
        player:sendMessage("Steel Mod active")
        log("Steel Mod: " .. player.name .. " joined")
    end)

    registerEvent("PlayerBlockBreakEvent", function(event)
        log("Steel Mod: " .. event.block.oid .. " broken at " .. tostring(event.block.pos))
    end)
end
