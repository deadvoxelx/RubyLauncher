-- Server scripts run when a world is hosted; this is where event listeners are registered
function main()
    registerEvent
	(	-- Fires on player join
		"PlayerJoinEvent", function(event)
        local player = event.player
        player:sendMessage("Steel Mod active")
        log("Steel Mod: " .. player.name .. " joined")
    	end
	)

    registerEvent
	(	-- Fires on block destroyed
		"PlayerBlockBreakEvent", function(event)
        log("Steel Mod: " .. event.block.oid .. " broken at " .. tostring(event.block.pos))
    	end
	)
	
    registerEvent
	(	-- Fires on item use completion
		"ItemCompleteUseEvent", function(event)
        if event.itemId ~= "steelMod:steelApple" then return end
        event.player:addEffect(Effect.Resistance, 2400, 0)
        event.player:addEffect(Effect.Regeneration, 100, 2)
    	end
	)
	
end