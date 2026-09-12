#include "LuaBindings.h"
#include "Loader.h"

#include "ServerPlayerGameMode.h"
#include "ServerPlayer.h"
#include "PlayerList.h"
#include "MinecraftServer.h"
#include "LevelSettings.h"
#include "CommandDispatcher.h"
#include "Level.h"
#include "PlayerConnection.h"
#include "Tile.h"

/* Server Includes */

#include "PlayerAbilitiesPacket.h"
#include "ServerLevel.h"
#include "Item.h"

#include "Registry/Item/ItemRegistry.h"
#include "Registry/Item/ItemFactory.h"
#include "Registry/Block/BlockRegistry.h"
#include "Registry/IDs.h"

#include "Server/Events/Item/ItemInteractEvent.h"
#include "Server/Events/Player/PlayerBlockBreakEvent.h"
#include "Server/Events/Player/PlayerBlockPlaceEvent.h"
#include "Server/Events/Player/PlayerConnectionEvent.h"
#include "Server/Events/Player/PlayerJoinEvent.h"
#include "Server/Events/Player/PlayerFlightStartedEvent.h"
#include "Server/Events/Player/PlayerFlightEndedEvent.h"

#include "Common/CactusUtils.h"
#include "LuaStructs.h"


void LuaBindings::bindCommonFunctions(const std::vector<sol::state*> &luaStates) {
    for (sol::state* lua : luaStates) {
        lua->set_function("log", [](const std::string &message) {
            Loader::log(message);
        });

        lua->new_usertype<LuaVec3>("Vec3",
            sol::constructors<LuaVec3(double, double, double)>(),
            "x", &LuaVec3::x,
            "y", &LuaVec3::y,
            "z", &LuaVec3::z,
            "distanceTo", &LuaVec3::distanceTo,
            sol::meta_function::to_string, &LuaVec3::toString,
            sol::meta_function::addition, &LuaVec3::addition,
            sol::meta_function::subtraction, &LuaVec3::subtraction,
            sol::meta_function::multiplication, &LuaVec3::multiplication,
            sol::meta_function::division, &LuaVec3::division
        );

        lua->new_usertype<LuaBlock>("Block",
            "pos", &LuaBlock::pos,
            "id", sol::property(&LuaBlock::getID),
            "oid", sol::property(&LuaBlock::getOID)
        );
    }
}

void LuaBindings::bindServerEvents(sol::state& lua) {
    lua.set_function("registerEvent",
        [](const std::string& event_name, sol::function callback) {
            EventBus::Get().registerListener(event_name, sol::protected_function(std::move(callback)));
        }
    );

    lua.new_usertype<CancellableCactusEvent>("CancellableCactusEvent",
        "setCancelled", &CancellableCactusEvent::setCancelled,
        "isCancelled", &CancellableCactusEvent::isCancelled,
        sol::base_classes, sol::bases<CactusEvent>()
    );

    lua.new_usertype<CactusEvent>("CactusEvent",
        "name", &CactusEvent::eventName
    );

    lua.new_usertype<Inventory>("Inventory",
        "setItem", [](Inventory& inv, const int slot, int count, const std::string& identifier, sol::this_state state) {
            IDMapping::MappedItem mapping = IDMapping::get()->getID(identifier);
            if (mapping.id == 0 && identifier != "minecraft:air") {
                CactusUtils::LuaException(state, "Identifier " + identifier + " does not exist");
                return;
            }
            if (Item::items[mapping.id] == nullptr) {
                CactusUtils::LuaException(state, "Item id " + std::to_string(mapping.id) + " is null in Item::items");
                return;
            }
            if (count <= 0) {
                CactusUtils::LuaException(state, "Item count is zero");
                return;
            }

            inv.setItem(slot, std::make_shared<ItemInstance>(mapping.id, count, mapping.aux));
        },
        "getItem", [](Inventory& inv, const int slot) {
            return inv.getItem(slot);
        },
        "clear", [](Inventory& inv) {
            inv.clearInventory(-1, -1);
        }
    );

    lua.new_usertype<ItemInstance>("ItemInstance",
        "getHoverName", [](ItemInstance& item) {
            return item.getHoverName();
        },
        "isStackable", &ItemInstance::isStackable,
        "isEnchanted", &ItemInstance::isEnchanted,
        "isDamaged", &ItemInstance::isDamaged,
        "damageValue", &ItemInstance::getDamageValue,
        "maxDamage", &ItemInstance::getMaxDamage,
        "getAmount", &ItemInstance::count,
        "id", &ItemInstance::id
    );

    lua.new_usertype<ServerPlayerGameMode>("ServerPlayerGameMode",
        "isSurvival", sol::property(&ServerPlayerGameMode::isSurvival),
        "isCreative", sol::property(&ServerPlayerGameMode::isCreative)
    );

    lua["GameMode"] = sol::table(lua, sol::create);
    lua["GameMode"]["NOT_SET"]   = 0;
    lua["GameMode"]["SURVIVAL"]  = 0;
    lua["GameMode"]["CREATIVE"]  = 1;
    lua["GameMode"]["ADVENTURE"] = 2;

    lua.new_usertype<ServerPlayer>("ServerPlayer",
        "getHeldItem", &ServerPlayer::getCarriedItem,
        "setFoodLevel", [](ServerPlayer& player, int food) {
            player.getFoodData()->setFoodLevel(food);
        },
        "feed", [](ServerPlayer& player, int food, double modifier) {
            player.getFoodData()->eat(food,modifier);
        },
        "getHealth", [](ServerPlayer& player) {
            return player.getHealth();
        },
        "setHealth", [](ServerPlayer& player, int health) {
            player.setHealth(health);
        },
        "pos", sol::property([](ServerPlayer& player) { return LuaVec3(player.x, player.y, player.z); }),
        "teleport", [](ServerPlayer& player, sol::object target, sol::this_state state) {
            if (target.is<LuaVec3>()) {
                auto vec3 = target.as<LuaVec3>();
                player.teleportTo(vec3.x, vec3.y, vec3.z);
            }else {
                CactusUtils::LuaException(state, "Not a valid Vec3 object");
            }
        },
        "setCanFly", [](ServerPlayer& player, bool toggle) {
            // Toggles the PERMISSION to fly
            unsigned int val = toggle == true ? 1 : 0;
            player.setPlayerGamePrivilege(Player::EPlayerGamePrivileges::ePlayerGamePrivilege_CanFly,val);
            player.abilities.mayfly = toggle;
            std::shared_ptr<PlayerAbilitiesPacket> pkt =
                std::make_shared<PlayerAbilitiesPacket>(&player.abilities);
            player.connection->send(pkt);
        },
        "setFly", [](ServerPlayer& player, bool toggle) {
            // Toggles the flight state (must implicitly toggle the permission if starting flight aswell!)
            if (toggle) player.abilities.mayfly = toggle; // If we are disabling flight; do not remove toggle. That is the job of toggleFlightAllowed
            player.abilities.flying = toggle;
            std::shared_ptr<PlayerAbilitiesPacket> pkt =
                std::make_shared<PlayerAbilitiesPacket>(&player.abilities);
            player.connection->send(pkt);
        },
        "canFly", [](ServerPlayer& player) {
            return player.abilities.mayfly;
        },
        "getViewDistance", [](ServerPlayer& player) {
            return player.getViewDistance();
        },
        "displayName", &ServerPlayer::m_displayName,
        "experience",  &ServerPlayer::experienceProgress,
        "flySpeed", sol::property([](ServerPlayer& p) { return p.abilities.getFlyingSpeed(); }),
        "level", &ServerPlayer::experienceLevel,
        "name", &ServerPlayer::name,
        "walkSpeed", sol::property([](ServerPlayer& p) { return p.abilities.getWalkingSpeed(); }),
        "giveExp", [](ServerPlayer& player, float amount) {
            player.experienceProgress += amount;
        },
        "giveExpLevels", [](ServerPlayer& player, int amount) {
            player.experienceLevel += amount;
        },
        "setExp", [](ServerPlayer& player, float amount) {
            player.experienceProgress = amount;
        },
        "setLevel", [](ServerPlayer& player, int amount) {
            player.experienceLevel = amount;
        },
        "gamemode", &ServerPlayer::gameMode,
        "inventory", &ServerPlayer::inventory,
        "setGameMode", [](ServerPlayer& player, int gameTypeId) {
            if (GameType* type = GameType::byId(gameTypeId)) player.setGameMode(type);
        },
        "sendMessage", [](ServerPlayer& p, const std::string& message) {
            std::wstring wmessage(message.begin(), message.end());
            p.sendMessage(wmessage);
        },
        "destroyBlock", [](ServerPlayer& p, sol::object target, sol::this_state state) {
            if (target.is<LuaVec3>()) {
                auto vec3 = target.as<LuaVec3>();
                if (p.level->getTile(vec3.x,vec3.y,vec3.z) == 0) return;
                p.gameMode->destroyBlock(vec3.x, vec3.y, vec3.z);
            }else {
                CactusUtils::LuaException(state, "Not a valid Vec3 object");
            }
        }
    );

    lua.new_usertype<PlayerBlockBreakEvent>("PlayerBlockBreakEvent",
        "player", &PlayerBlockBreakEvent::player,
        "block", &PlayerBlockBreakEvent::block,
        sol::base_classes, sol::bases<CancellableCactusEvent, CactusEvent>()
    );

    lua.new_usertype<PlayerBlockPlaceEvent>("PlayerBlockPlaceEvent",
        "player", &PlayerBlockPlaceEvent::player,
        "block", &PlayerBlockPlaceEvent::block,
        sol::base_classes, sol::bases<CancellableCactusEvent, CactusEvent>()
    );

    lua.new_usertype<ItemInteractEvent>("ItemInteractEvent",
        "item", &ItemInteractEvent::item,
        //"level", &ItemInteractEvent::level, // We need to implement a usertype for level
        "player", &ItemInteractEvent::player,
        sol::base_classes, sol::bases<CactusEvent>()
    );

    lua.new_usertype<PlayerConnectionEvent>("PlayerConnectionEvent",
        "player", &PlayerConnectionEvent::player,
        sol::base_classes, sol::bases<CactusEvent>()
    );

    lua.new_usertype<PlayerJoinEvent>("PlayerJoinEvent",
        "player", &PlayerJoinEvent::player,
        sol::base_classes, sol::bases<CactusEvent>()
    );

    lua.new_usertype<PlayerFlightStartedEvent>("PlayerFlightStartedEvent",
        "player", &PlayerFlightStartedEvent::player,
        sol::base_classes, sol::bases<CactusEvent>()
    );

    lua.new_usertype<PlayerFlightEndedEvent>("PlayerFlightEndedEvent",
        "player", &PlayerFlightEndedEvent::player,
        sol::base_classes, sol::bases<CactusEvent>()
    );
}

void LuaBindings::bindServerFunctions(sol::state& lua, MinecraftServer* server) {
    lua["server"] = server;

    lua.new_usertype<MinecraftServer>("MinecraftServer",
        "getCommandDispatcher", &MinecraftServer::getCommandDispatcher,
        "getPlayers", [](MinecraftServer* server) -> std::vector<std::shared_ptr<ServerPlayer>>& {
            return server->getPlayers()->players;
        }
    );

    lua.new_usertype<CommandSender>("CommandSender",
        "hasPermission", &CommandSender::hasPermission
    );

    lua.new_usertype<Player>("Player",
        "getPos", &Player::getPos,
        "setPos", sol::resolve<void(double, double, double)>(&Player::setPos),
        "abilities", &Player::abilities,
        "changeDimension", &Player::changeDimension,
        "sendMessage", &Player::sendMessage,
        sol::base_classes, sol::bases<CommandSender>()
    );

    lua.new_usertype<CommandDispatcher>("CommandDispatcher",
        "performCommand", &CommandDispatcher::performCommand
    );

    lua.new_enum<EGameCommand>("EGameCommand", {
            {"COUNT", eGameCommand_COUNT},
            {"EnchantItem", EGameCommand::eGameCommand_EnchantItem},
            {"Experience", EGameCommand::eGameCommand_Experience},
            {"GameMode", EGameCommand::eGameCommand_GameMode},
            {"Give", EGameCommand::eGameCommand_Give},
            {"Kill", EGameCommand::eGameCommand_Kill},
            {"Teleport", EGameCommand::eGameCommand_Teleport},
            {"Time", EGameCommand::eGameCommand_Time},
            {"ToggleDownfall", EGameCommand::eGameCommand_ToggleDownfall}
        }
    );

    lua.new_usertype<ServerLevel>("ServerLevel",
        "getBlocksAndData", &ServerLevel::getBlocksAndData,
        "setBlocksAndData", &ServerLevel::setBlocksAndData,
        "getServer", &ServerLevel::getServer,
        "isClientside", &ServerLevel::isClientSide,
        "getTile", &ServerLevel::getTile,
        "hasChunkAt", &ServerLevel::hasChunkAt
    );
}

void LuaBindings::bindClientFunctions(sol::state& lua) {
    lua.new_enum<EBaseItem>("EBaseItem", {
        {"Default", EBaseItem::Default},
        {"Food", EBaseItem::Food},
        {"Hoe", EBaseItem::Hoe},
        {"Weapon", EBaseItem::Weapon},
        {"Pickaxe", EBaseItem::Pickaxe},
        {"Hatchet", EBaseItem::Hatchet},
        {"Shovel", EBaseItem::Shovel}
    });

    lua.new_usertype<ItemDefinition>("ItemDefinition",
        sol::constructors<ItemDefinition(sol::table)>(),
        "type", &ItemDefinition::type,
        "nutrition", &ItemDefinition::nutrition,
        "saturationMod", &ItemDefinition::saturationMod,
        "isMeat", &ItemDefinition::isMeat,
        "tier", &ItemDefinition::tier
    );

    lua.new_usertype<Item::Tier>("Tier",
        sol::constructors<Item::Tier(int,int,double,int,int)>(),
        "getUses", &Item::Tier::getUses,
        "getLevel", &Item::Tier::getLevel
    );

    lua.set_function("registerItem", [](sol::this_environment env,const std::string& id, const std::string& name, const std::string& texturePath, const ItemDefinition& def) {
        sol::environment& modEnv = env;
        std::string envPath = modEnv["pathName"];
        std::string modId = modEnv["modId"];
        std::wstring path = std::wstring(envPath.begin(), envPath.end());
        return ItemRegistry::registerItem(path, id, name, modId, def, texturePath);
    });

    lua.set_function("registerBlock", [](sol::this_environment env,const std::string& id, const std::string& name, const std::string& texturePath, sol::this_state state) {
        sol::environment& modEnv = env;
        std::string envPath = modEnv["pathName"];
        std::string modId = modEnv["modId"];
        std::wstring path = std::wstring(envPath.begin(), envPath.end());

        int registeredBlock = BlockRegistry::registerBlock(path, id, name, modId, texturePath);
        if (registeredBlock == -1) {
            CactusUtils::LuaException(state, "The block registry limit has been reached, can't register more than 81 custom blocks");
            return -1;
        }
        return registeredBlock;
    });
}