#include "Loader.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
#include <string>

#include "Client/Rendering/ModTextureAtlas.h"
#include "Lua/LuaBindings.h"

class MinecraftServer;

namespace fs = std::filesystem;
using string = std::string;

Loader::Loader() {
    ModTextureAtlas::createInstance();
    luaServer.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::package, sol::lib::math);
    luaClient.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::package, sol::lib::math);

    auto panicHandler = [](lua_State* L) -> int {
        fprintf(stderr, "[Lua] PANIC: %s\n", lua_tostring(L, -1));
        return 0;
    };
    lua_atpanic(luaServer.lua_state(), panicHandler);
    lua_atpanic(luaClient.lua_state(), panicHandler);

    if (!fs::exists("mods")) {
        fs::create_directory("mods/");
    }

    LuaBindings::bindCommonFunctions({ &luaServer, &luaClient });
    LuaBindings::bindClientFunctions(luaClient);
    LuaBindings::bindServerEvents(luaServer);
    app.DebugPrintf("Cactus ModLoader initialized!\n");
}

void Loader::_debugPrint(const string &output) {
    app.DebugPrintf(("[Cactus ModLoader] "+output+"\n").c_str());
}

nlohmann::json Loader::getManifest(const string &filePath) {
    string fullPath = filePath + "/manifest.json";
    auto data = loadFile(fullPath);
    return nlohmann::json::parse(data);
}

void Loader::log(const string& message) {
    app.DebugPrintf(("[Cactus ModLoader] " + message + "\n").c_str());
}

string Loader::loadFile(string fileName) {
    std::ifstream file(fileName);
    stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

void Loader::registerServerFunctions(MinecraftServer *server) {
    LuaBindings::bindServerFunctions(luaServer, server);
}

bool hasMetadata(nlohmann::json& json, const std::string& key) {
    return !json[key].is_null();
}

void Loader::collectMods() {
    //iterate through mods folder
    for (const auto & modEntry : fs::directory_iterator("mods/")) {
        if (!modEntry.is_directory()) continue;
        string modPath = modEntry.path().string();

        nlohmann::json json;
        try {
            json = getManifest(modEntry.path().string());
        } catch (const std::exception& e) {
            _debugPrint("Could not read manifest.json for '"+modPath+"' exception: "+e.what());
            continue;
        }
        
        CactusMod m;
        bool missingRequired = false;

        for ( const auto & metadata : requiredMetadata ) {
            if (!hasMetadata(json, metadata)) {
                _debugPrint((modPath+" missing metadata '"+metadata+"' please add it to your manifest.json"));
                missingRequired = true;
                break;
            }
            m.metadata[metadata] = json[metadata].get<std::string>();
        }

        if (missingRequired) continue;

        string folderName = modEntry.path().filename().string();
        m.setPathName(folderName);
        mods_[m.getName()] = m;
    }
}

void Loader::refresh(sol::state& luaState,std::string_view (CactusMod::*getEntry)() const,bool prependPath) {
    std::vector<CactusMod> failedMods;

    for (auto& [name,mod] : mods_) {
        std::string modName = std::string(mod.getName());
        std::string modId = std::string(mod.getModID());
        std::string folderName = std::string(mod.getPathName());
        std::string modEntry = std::string((mod.*getEntry)());

        std::string scriptPath = "mods/" + folderName + "/" + modEntry;
        if (!fs::exists(scriptPath)) {
            _debugPrint("Could not find file for " + scriptPath + " for mod " + modName);
            continue;
        }

        if (prependPath) {
            std::string currentPath = luaState["package"]["path"];
            luaState["package"]["path"] = "mods/" + folderName + "/?.lua;" + currentPath;
        }

        sol::environment modEnv(luaState, sol::create, luaState.globals());
        modEnv["modId"] = modId;
        modEnv["pathName"] = folderName;

        sol::protected_function_result result;
        try {
            result = luaState.safe_script_file(scriptPath, modEnv, sol::script_pass_on_error);
        } catch (const sol::error& e) {
            _debugPrint("There is a lua error in '"+folderName+"/"+modEntry+"' error: "+e.what());
            failedMods.push_back(mod);
            continue;
        }

        if (result.valid()) {
            if (&luaState == &luaServer) {
                mod.setServerEnv(std::move(modEnv));
            } else {
                mod.setClientEnv(std::move(modEnv));
            }
            _debugPrint(modId + "'s '" + modEntry + "' has been loaded successfully");
        } else {
            sol::error err = result;
            _debugPrint("There is a lua error in '"+folderName+"/"+modEntry+"' error: "+err.what());
            failedMods.push_back(mod);
        }
    }

    for (const auto& failedMod : failedMods) {
        _debugPrint("Unloading '"+std::string(failedMod.getName())+"' due to script error");
        mods_.erase(failedMod.getName());
    }
}

void Loader::refreshServerScripts() {
    refresh(luaServer,&CactusMod::getServerEntry,true);
}

void Loader::refreshClientScripts() {
    refresh(luaClient,&CactusMod::getClientEntry,false);
}

void Loader::execute(sol::environment& (CactusMod::*getEnv)(), std::string funcName, bool warn) {
    for (auto& [name, mod] : mods_) {
        std::string modName = std::string(mod.getName());

        sol::environment& env = (mod.*getEnv)();
        if (!env.valid()) continue;

        sol::protected_function fn = env[funcName];

        if (fn.valid()) {
            auto result = fn();
            if (!result.valid()) {
                sol::error err = result;
                _debugPrint(("Error in '"+modName+"': "+err.what()).c_str());
            }
        } else {
            if (!warn) return;
            _debugPrint(("Mod "+modName+" must have a '"+funcName+"()' function in its global table, 'function "+modName+"."+funcName+"()' is missing"));
        }
    }
}

void Loader::executeServerScripts(std::string name, bool warn) {
    execute(&CactusMod::getServerEnv, name, warn);
}

void Loader::executeClientScripts(std::string name, bool warn) {
    execute(&CactusMod::getClientEnv, name, warn);
}
