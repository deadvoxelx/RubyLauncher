function main()
    -- Blocks
    local blockId = registerBlock
	(	-- ID --		-- Name --		-- Texture --
		"steelBlock", "Steel Block", "res/steel_block.png", 
		BlockDefinition.new
		(
			{
				hardness = 5.0, 
				resistance = 15.0, 
				tool = EBlockTool.Pickaxe
			}
		)
	)
    log("Steel Mod: registered steelBlock as id " .. tostring(blockId))

	-- Basic Items
    local gemId = registerItem
	(
		"steelIngot", "Steel Ingot", "res/steel_ingot.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Default
			}
		)
	)
    log("Steel Mod: registered steelIngot as id " .. tostring(gemId))

	-- Foods
	-- saturationMod guideline: 0.1 poor, 0.3 low, 0.6 normal, 0.8 good, 1.0 max, 1.2 supernatural
	local pearId = registerItem
	(
		"pear", "Pear", "res/pear.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Food, 
				nutrition = 4, 
				saturationMod = 0.3, 
				isMeat = false
			}
		)
	)
	log("Steel Mod: registered pear as id " .. tostring(pearId))

    -- Tools
    local swordId = registerItem
	(
		"steelSword", "Steel Sword", "res/steel_sword.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Weapon, 
				tier = EItemTier.Iron
			}
		)
	)
    log("Steel Mod: registered steelSword as id " .. tostring(swordId))
	
	local hatchetId = registerItem
	(
		"steelAxe", "Steel Axe", "res/steel_axe.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Hatchet, 
				tier = EItemTier.Iron
			}
		)
	)
    log("Steel Mod: registered steelAxe as id " .. tostring(hatchetId))
	
	local pickaxeId = registerItem
	(
		"steelPickaxe", "Steel Pickaxe", "res/steel_pickaxe.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Pickaxe, 
				tier = EItemTier.Iron
			}
		)
	)
    log("Steel Mod: registered steelPickaxe as id " .. tostring(pickaxeId))
	
	local shovelId = registerItem
	(
		"steelShovel", "Steel Shovel", "res/steel_shovel.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Shovel, 
				tier = EItemTier.Iron
			}
		)
	)
    log("Steel Mod: registered steelShovel as id " .. tostring(shovelId))
	
	local hoeId = registerItem
	(
		"steelHoe", "Steel Hoe", "res/steel_hoe.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Hoe, 
				tier = EItemTier.Iron
			}
		)
	)
    log("Steel Mod: registered steelHoe as id " .. tostring(hoeId))

	-- Armor
	local helmetId = registerItem
	(
		"steelHelmet", "Steel Helmet", "res/steel_armor_helmet.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Helmet, 
				armorMaterial = EArmorMaterial.Iron, 
				armorSet = "steel"
			}
		)
	)
    log("Steel Mod: registered steelHelmet as id " .. tostring(helmetId))

	local chestplateId = registerItem
	(
		"steelChestplate", "Steel Chestplate", "res/steel_armor_chestplate.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Chestplate, 
				armorMaterial = EArmorMaterial.Iron, 
				armorSet = "steel"
			}
		)
	)
    log("Steel Mod: registered steelChestplate as id " .. tostring(chestplateId))

	local leggingsId = registerItem
	(
		"steelLeggings", "Steel Leggings", "res/steel_armor_leggings.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Leggings, 
				armorMaterial = EArmorMaterial.Iron, 
				armorSet = "steel"
			}
		)
	)
    log("Steel Mod: registered steelLeggings as id " .. tostring(leggingsId))

	local bootsId = registerItem
	(
		"steelBoots", "Steel Boots", "res/steel_armor_boots.png", 
		ItemDefinition.new
		(
			{
				base = EBaseItem.Boots, 
				armorMaterial = EArmorMaterial.Iron, 
				armorSet = "steel"
			}
		)
	)
    log("Steel Mod: registered steelBoots as id " .. tostring(bootsId))
end
