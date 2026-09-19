-- Server scripts run when a world is hosted; this is where event listeners are registered
-- Worldgen stuff is also done here
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
	
	-- Ore Features
	registerOreFeature
	(
		"steelOreFeature",								-- Feature ID
		"steelMod:steelOre",							-- Placed block
		{
			size = 11,									-- Blocks per vein
			target = "minecraft:stone",					-- Replaced block
			dimension = 0,								-- Dimension (-999 for all)
			yMin = 0, yMax = 64,						-- Min + max y
			count = 8,									-- Average veins per chunk
		}
	)

	-- Tree Features
	registerTreeFeature
	(
		"steelTreeFeature",								-- Feature ID
		"oak",									-- Tree shape (oak, spruce, birch, hugeJungle, swamp)
		{
			trunk = "steelMod:steelLog",				-- Log
			leaves = "steelMod:steelLeaves",			-- Leaves
			biome = "minecraft:forest",					-- Biome
			count = 1,									-- Average trees per chunk
			height = 7,									-- Base height
		}
	)
end