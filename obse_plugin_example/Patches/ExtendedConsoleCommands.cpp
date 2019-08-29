#include "Patches/ExtendedConsoleCommands.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h" // for camera state vars
#include "Miscellaneous/strings.h"
#include "obse_common/SafeWrite.h"
#include "obse/GameAPI.h" // g_bConsoleMode
#include "obse/GameMenus.h"
//
#include "Services/INISettings.h"
#include "Services/TileDump.h"
#include "Selectors.h"

#include "ReverseEngineered/Miscellaneous.h" // quest testing
#include "obse/GameData.h" // quest testing

namespace CobbPatches {
   namespace ExtendedConsoleCommands {
      //
      // Not every console command is a script command. There are specific commands hardcoded 
      // into the console, such as "reload %s" to reload a menu. Why not add our own, with a 
      // syntax that should ensure they don't conflict with anything? We'll require that all 
      // of our commands be prefixed with "!xxn ".
      //
      // Figure this could be useful for debugging.
      //
      // Here's a list of the vanilla commands. They're generally parsed by using a string 
      // compare first, and any arguments are extracted using sscanf; as a consequence of 
      // this, you *generally* can't create new commands that start with the same substrings 
      // (but since we use the !xxn prefix, that doesn't apply to us). Note that these are 
      // also less polished than real script commands; for example, entering "set trait" 
      // without any arguments causes a crash.
      //
      //    dof %f                    | Unknown.
      //    usz %i                    | Sets the screen safe zone size. Takes two args?
      //    clr                       | Clears the command console.
      //    cls                       | Clears the command console.
      //    tag                       | Makes an object render its formID as in-world text.
      //    reload strings            | Reloads the strings.xml file.
      //    reload HUDReticle         | Reloads the HUDReticle menu.
      //    reload HUDSafeZone        | Reloads the HUDSafeZone menu.
      //    TEH                       | Toggles visibility of the enemy health display.
      //    reload %s                 | Reloads a menu by name. Command name is "reload ".
      //    delete %s                 | Unknown. Command name is "delete ".
      //    front                     | Echoes the name of the frontmost menu's root tile.
      //    stack                     | Echoes a list of all "active" menus' IDs.
      //    close all menus           | Attempts to forcibly close all "active" menus.
      //    visible %i                | Checks if the specified menu ID is open and visible.
      //    exit menus                | Unknown.
      //    emergency texture release | Unknown.
      //    playerpos %f %f %f        | 
      //    repair %i                 | Opens the RepairMenu.
      //    set trait %s %s %f        | 
      //    depth                     | Shows the depth of the frontmost tile in the UI.
      //    new fonts                 | Sets a flag in the FontManager.
      //    old fonts                 | Clears a flag in the FontManager.
      //    use workbook              | Unknown. The BookMenu must already be open.
      //    bat %s                    | Executes a batch file.
      //
      namespace HACK_ForceLogAllCommandResults {
         //
         // Add an extended command that can force the console to write the results of all 
         // executed commands to our log file. This is meant to deal with certain commands, 
         // particularly many OBSE commands, not logging their results when called from the 
         // console.
         //
         static bool s_enabled = false;
         //
         void _stdcall Inner(double* r, UInt16* opcodeOffset) {
            if (!*g_bConsoleMode)
               return;
            if (!s_enabled)
               return;
            _MESSAGE("Command produced result: %f", *r);
         }
         __declspec(naked) void Outer() {
            _asm {
               test al, al;
               jz   lExit;
               lea  ecx, [esp + 0x2C];
               lea  eax, [esp + 0x14];
               push eax;
               push ecx;
               call Inner;
               mov  eax, 1;
            lExit:
               pop  ecx; // reproduce patched-over pops
               pop  edi;
               pop  esi;
               pop  ebp;
               pop  ebx;
               mov  ecx, 0x005172E8;
               jmp  ecx;
            }
         }
         void Apply() {
            WriteRelJump(0x005172E3, (UInt32)&Outer);
         }
      }
      namespace Commands {
         void activeTileID() {
            auto tile = RE::InterfaceManager::GetInstance()->activeTile;
            if (tile) {
               _MESSAGE("activeTileID: %f", CALL_MEMBER_FN(tile, GetFloatTraitValue)(RE::kTagID_id));
            } else
               _MESSAGE("activeTileID: No tile.");
         }
         void cameraState() {
            _MESSAGE("cameraState: yaw %f, pitch %f", *RE::fPlayerCameraYaw, *RE::fPlayerCameraPitch);
         }
         void genericTestCommand() { // modify this to test whatever we need to test
            _MESSAGE("genericTestCommand: Running...");
            //
            _MESSAGE("No test has been written for the generic test command.");
            //
            _MESSAGE("genericTestCommand: Done.");
         }
         void refreshINI() {
            NorthernUI::INI::INISettingManager::GetInstance().Load();
         }
         void setForceLog(const char* args) {
            bool flag = false;
            if (args[0] == '\0')
               flag = true;
            else {
               int a;
               sscanf_s(args + 1, "%d", &a);
               if (a != 0)
                  flag = true;
            }
            HACK_ForceLogAllCommandResults::s_enabled = flag;
         }
         void tileInfo(const char* args) {
            constexpr char ce_selectorDescendant = '@';
            //
            if (!args || args[0] != ' ') {
               _MESSAGE("tileInfo: You must specify a tile to search for, optionally preceded by a menu ID (decimal) and space.");
               return;
            }
            args = args + 1;
            if (args[0] == '\0') {
               _MESSAGE("tileInfo: You must specify a tile to search for, optionally preceded by a menu ID (decimal) and space.");
               return;
            }
            SInt32      menuID = 0;
            const char* name   = args;
            RE::Tile*   tile   = nullptr;
            if (_stricmp(name, "northernui()") == 0) {
               tile = g_northernUIDatastore;
            } else if (_stricmp(name, "xxnstrings()") == 0) {
               tile = g_northernUIStringstore;
            } else if (_stricmp(name, "xxnlocalization()") == 0) {
               tile = g_northernUILocConfigTile;
            } else if (_stricmp(name, "xxnprefs()") == 0) {
               tile = g_northernUIPrefstore;
            } else {
               {
                  char* end = nullptr;
                  menuID = strtoul(args, &end, 10);
                  if (!(end && end != args && *end == ' ')) {
                     menuID = 0;
                  } else {
                     name = end + 1;
                  }
               }
               auto ui = RE::InterfaceManager::GetInstance();
               tile = ui->menuRoot;
               if (menuID) {
                  menuID -= 0x3E9;
                  if (menuID > 0 && menuID < g_TileMenuArray->capacity) {
                     TileMenu* menu = g_TileMenuArray->data[menuID];
                     if (menu)
                        tile = (RE::Tile*)menu;
                  }
               }
               RE::Tile* target = nullptr;
               std::string full = name;
               std::string segment;
               size_t from  = 0;
               size_t until = std::string::npos;
               while ((until = full.find(ce_selectorDescendant, from)) != std::string::npos) {
                  segment = full.substr(from, until - from);
                  if (!segment.size()) {
                     _MESSAGE("tileInfo: bad path (blank component at char %d).", from);
                     _MESSAGE("   %s", full.substr(0, from + 1));
                     std::string line;
                     line.resize(from, '-');
                     line += '^';
                     _MESSAGE("   %s", line.c_str());
                     return;
                  }
                  if (strcmp(tile->name.m_data, segment.c_str()) == 0) {
                     target = tile;
                     break;
                  }
                  tile = RE::GetDescendantTileByName(tile, segment.c_str());
                  from = until + 1;
                  if (!tile)
                     break;
               }
               if (tile && !target) { // handle string leftovers
                  segment = full.substr(from);
                  if (segment.size()) {
                     if (strcmp(tile->name.m_data, segment.c_str()) == 0)
                        target = tile;
                     else
                        target = RE::GetDescendantTileByName(tile, segment.c_str());
                  } else
                     target = tile;
               }
               tile = target;
               /*// code for not allowing '@' to specify paths
               if (!tile->name.m_data || strcmp(tile->name.m_data, name) != 0)
                  tile = RE::GetDescendantTileByName(tile, name);
               //*/
            }
            if (tile) {
               _MESSAGE("tileInfo: tile %s found.", tile->name.m_data);
               TileDump(tile);
            } else {
               _MESSAGE("tileInfo: tile %s not found.", name);
            }
         }
      }
      namespace Hook {
         bool _startsWith(const char* a, const char* b) {
            return !_strnicmp(a, b, cobb::cstrlen(b));
         }
         //
         bool _stdcall Inner(const char* input) {
            if (!_startsWith(input, "!xxn "))
               return false;
            if (_startsWith(input + cobb::cstrlen("!xxn "), "activeTileID")) {
               Commands::activeTileID();
               return true;
            }
            if (_startsWith(input + cobb::cstrlen("!xxn "), "cameraState")) {
               Commands::cameraState();
               return true;
            }
            if (_startsWith(input + cobb::cstrlen("!xxn "), "genericTestCommand")) {
               Commands::genericTestCommand();
               return true;
            }
            if (_startsWith(input + cobb::cstrlen("!xxn "), "refreshINI")) {
               Commands::refreshINI();
               return true;
            }
            if (_startsWith(input + cobb::cstrlen("!xxn "), "setForceLog")) {
               Commands::setForceLog(input + cobb::cstrlen("!xxn ") + cobb::cstrlen("setForceLog"));
               return true;
            }
            if (_startsWith(input + cobb::cstrlen("!xxn "), "tileInfo")) {
               Commands::tileInfo(input + cobb::cstrlen("!xxn ") + cobb::cstrlen("tileInfo"));
               return true;
            }
         }
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x005723E0;
               call eax; // reproduce patched-over call
               lea  eax, [esp + 0x158];
               push eax;
               call Inner;
               test al, al;
               jz   lNormal;
               mov  eax, dword ptr [esp + 0x28]; // eax = (/*BSStringT*/esp28).m_data;
               push eax;
               mov  eax, 0x00401F20; // FormHeap_Free
               call eax;
               add  esp, 4; // clean up after last non-member call
               mov  eax, 1;
               mov  ecx, 0x0058740B;
               jmp  ecx;
            lNormal:
               mov  eax, 0x005862C6;
               jmp  eax;
            }
         }
         void Apply() {
            WriteRelJump(0x005862C1, (UInt32)&Outer);
         }
      }
      //
      void Apply() {
         Hook::Apply();
         HACK_ForceLogAllCommandResults::Apply();
      };
   };
};