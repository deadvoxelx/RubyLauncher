# Ruby Launcher
A fork-agnostic and cross-platform Mod Loader for LCE.

Ruby Launcher allows you to mod LCE using lua scripting.

# Scope
The scope of this project is to have a fork-agnostic and cross-platform Mod Loader for LCE

Goals: 

* Add custom block and item registration with custom textures (implemented)
* Event system (implemented)
* Client and server modding using LUA scripting (implemented)
* Allow for modding the UI
* Custom generation and dimensions
* Port Cactus ModLoader to Minecraft Java Edition
* String ID system for items and blocks (implemented)
* More...

# Building
### Dependencies
This project requires Lua 5.4 and its developments headers for compiling 

Ubuntu
```bash
sudo apt install lua5.4 liblua5.4-dev
```

Arch Linux
```bash
sudo pacman -S lua
```

Fedora
```bash
sudo dnf install lua lua-devel
```

Windows: I suggest using the [vcpkg](https://vcpkg.io/en/) package manager to install the dependencies or you can download the pre-compiled binaries and headers on the [LuaBinaries](https://luabinaries.sourceforge.net/download.html) website.
```PowerShell
vcpkg install lua:x64-windows
```


# Implementing and building
Ruby Launcher is made to be fork-agnostic so that it could be implemented into any fork of LCE

You are responsible for maintaining a custom implementation of Ruby Launcher on a LCE fork while respecting the contribution policy of the project

The project comes with a meson.build file that can be added as a dependency for the Minecraft.Client

# Contribution policy
Submitting code to this repository authored by generative AI tools (LLMs, agentic coding tools, etc...) is strictly forbidden
Pull requests that are clearly vibe-coded or written by an LLM will be closed

