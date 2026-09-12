function main()
	registerEvent("PlayerJoinEvent", function(event)
        local player = event.player
        player:sendMessage("hi bub")
    end)

    registerEvent("PlayerBlockPlaceEvent", function(event)
        event:setCancelled(true)
    end)

    registerEvent("PlayerBlockBreakEvent", function(event)
        event:setCancelled(true)
    end)
end
