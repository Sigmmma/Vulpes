This readme is still work in progress, and so is the mod.

<a href="COPYING.md"><img align="right" src="agplv3.png"></a>

# Vulpes
An opensource Halo Client and Server Extension Mod


# Introduction

Vulpes is a mod for Halo Custom Edition that aims to improve the capabilities of modders and to improve the online experience. It fixes bugs, increases limits, and adds features. It is intended to be a replacement for the closed source mod SAPP and in the long term Opensauce.

# Getting Started

When Vulpes releases it will be obtainable from the releases here for this github project. Very likely it will come with the VulpesLoader.dll which will keep the mod up-to-date.

Installing will be done by putting the VulpesLoader.dll inside of the game's controls folder, or the mods folder if you are using monolith. Or even strings.dll if you are using it as a server extension.

### Vulpes Directories

When Vulpes is loaded for the first time it will create a few directories inside of your Halo path directory. This is by default 'documents\\my games\\halo ce' but can be modified to be anywhere using the -path startup argument for Halo.

```
vulpes               - directory - root directory
│  └settings.ini     - text file - settings
│   
└───┬lua             - directory - lua scripts root
    ├───global       - directory - global lua scripts
    └───map          - directory - map specific lua scripts
```
There will be more directories and files here as the project expands.

# Features
Vulpes has a few features that are really important, but also a few small quality of life improvements.
### Extensive Lua API
Vulpes has a (currently still in development) modder focussed LUA API. It is intended to give you strong abilities without asking you to do much prepratation.
 - **Accessing memory structures via 'keys':** This allows you to read and write from structures in memory without needing an understanding of how they work. Everything can be accessed by 'keys' that describe what the values are, this lets vulpes know where to get the info, and/or where to write them to. This system makes it really easy for inexperienced modders to make their own scripts, and it also is really empowering for experienced modders as they will not be stuck trying to figure out memory structures and can just very easily make what they want to make.
 - **Event Callbacks:** There is also a lot of event callbacks that you are able to modify data from before they execute and get the outgoing data from when they're done. Some events can also be blocked, so if you want something specific not to happen you can stop it before it has the chance.
 - **Ability To Execute Certain Game Functions:** Another very liberating feature of this api is that there are many functions related to objects and other game elements, you for instance will be able to command a biped to jump, or perform a melee attack.
### Network
Vulpes will sync AI, and in the long term it will strengten the netcode to aid with making LUA script based gametypes and gameplay features and general gameplay more consistent \[for Vulpes Servers\].

    WIP
### Extended limits
    WIP
### Multiplayer Server Commands
For multiplayer a lot of commands are added to both mirror SAPP's featureset and give server admins strong abilities without needing to resort to lua scripts.

    WIP
 - **v_sv_hill_move:** Allows you to move the hill on demand.
 - **v_sv_hill_timer:** Allows you to set the time it will take for the hill to move.
### User Commands:
    WIP
### Quality of life
These are generally small changes with a noticeable impact.
 - **Faster Startup:** The game now only gets a checksum for a map once when it needs one, this makes starting the game near instant.
 - **Scoreboard Fade Removal:** The multiplayer scoreboard now appears instantly as on lower framerates it could take quite a few seconds for the text to be properly readable or fade out again.
 - **Console Always enabled:** This is so you won't need to add something to a shortcut, or use a shortcut at all to put in Vulpes Commands.
 - **Loading screen background removed:** This removes the loading screen background so you are not stuck waiting for it to fade out. It does not remove the text though, so you will still be able to read what the name of the map you're playing is.
### Bugfixes
Halo has some bugs that either came when porting the game onto PC or were there from the start.
 - **CPU Usage:** The game/server will now use less CPU time when idling.
 - **Console Fade:** The console text fade used to be affect by framerate, now it fades at the correct speed.
 - **HSC Print crashbug:** The original game's print command allowed for the use of formatted strings but did not allow you to give more than one argument. This would result in printing garbage or reading invalid memory. Causing the game to crash.
 - **Less Z-Fighting on Shader Transparent Geometry:** This helps with the flickery behavior of certain transparent shaders in maps like Bloodgulch and Assault on the Control Room.
