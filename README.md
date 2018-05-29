# NorthernUI
Source code for a DLL that modifies Oblivion's UI engine. The mod is currently available for distribution [on NexusMods](https://www.nexusmods.com/oblivion/mods/48577).

## Setup

Source code here depends on, but does not include, OBSE source files. Get those from their original source and fit them into the folder hierarchy in this project. Make sure this project's Visual Studio-specific stuff (e.g. solution files) overwrite those from OBSE. Everything's set up for Visual Studio Community 2015.

## Licensing
The code in the "obse_plugin_example" folder is mine unless otherwise stated; I retain whatever rights, permissions, etc., I have over it "naturally."

Everything in the "ReverseEngineered" folder constitutes information gathered while reverse-engineering Oblivion, with the exception of the "InGameDate" class defined in "ReverseEngineered/Systems/Timing.h". (I wrote that myself, forgot to move it to a better file, and will likely move it in an update.) As such, everything other than that class shall be considered as free and open as I'm allowed to make it; I place no limitations, restrictions, or requirements on the use of any code in the "ReverseEngineered" folder besides code pertaining to the "InGameDate" class.
