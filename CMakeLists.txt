include(FetchContent)

FetchContent_Declare(
  lua
  URL https://www.lua.org/ftp/lua-5.4.7.tar.gz
  URL_HASH SHA256=9fbf5e28ef86c69858f6d3d34eccc32e911c1a28b4120ff3e84aaa70cfbf1e30
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(lua)

set(LUA_SRC_DIR "${lua_SOURCE_DIR}/src")

file(GLOB LUA_CORE_SOURCES "${LUA_SRC_DIR}/*.c")
list(REMOVE_ITEM LUA_CORE_SOURCES "${LUA_SRC_DIR}/lua.c" "${LUA_SRC_DIR}/luac.c")

add_library(lua54 STATIC ${LUA_CORE_SOURCES})
target_include_directories(lua54 PUBLIC "${LUA_SRC_DIR}")
set_target_properties(lua54 PROPERTIES LINKER_LANGUAGE C)

file(GLOB_RECURSE CML_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(Cactus.ModLoader STATIC ${CML_SOURCES})

target_include_directories(Cactus.ModLoader PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)

target_include_directories(Cactus.ModLoader PRIVATE
  "${CMAKE_SOURCE_DIR}/Minecraft.Client"
  "${CMAKE_SOURCE_DIR}/Minecraft.Client/${PLATFORM_NAME}/Iggy/include"
  "${CMAKE_SOURCE_DIR}/Minecraft.World"
  "${CMAKE_SOURCE_DIR}/include/"
  "${CMAKE_BINARY_DIR}/generated/"
)

set(CML_DEFINES ${MINECRAFT_SHARED_DEFINES})
list(FILTER CML_DEFINES EXCLUDE REGEX "_HAS_STD_BYTE")
target_compile_definitions(Cactus.ModLoader PRIVATE ${CML_DEFINES} PUBLIC CACTUS_MODLOADER)
target_precompile_headers(Cactus.ModLoader PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/Minecraft.Client/stdafx.h>")
configure_compiler_target(Cactus.ModLoader)
target_link_libraries(Cactus.ModLoader PUBLIC lua54 PRIVATE Minecraft.World)
