#include "Selectors.h"
//
#include "Patches/TagIDs/Main.h"
#include "Patches/XboxGamepad/Main.h"
#include "ReverseEngineered/UI/Tile.h"
#include "Services/INISettings.h"
#include "Services/UIPrefs.h"

#include "obse/GameData.h" // FileFinder
#include "obse/GameTiles.h"
#include "obse_common/SafeWrite.h"

extern RE::Tile* g_northernUIDatastore     = nullptr;
extern RE::Tile* g_northernUIStringstore   = nullptr;
extern RE::Tile* g_northernUILocConfigTile = nullptr;
extern RE::Tile* g_northernUIPrefstore     = nullptr;

namespace CobbPatches {
   namespace Selectors {
      //
      // Selector ideas:
      //
      //    ancestor(1)       == returns ancestor tile by distance, such that ancestor(1) == parent() and ancestor(0) == me()
      //    childbyindex(123) == returns child by zero-indiced integer
      //    path(...)         == returns descendant tile by an xpath or CSS-style spec -- but it'd be limited to (255 - strlen("path()")) chars!
      //
      // Implemented selectors:
      //
      //    menu()       == returns tile's menu root
      //    northernui() == returns a NorthernUI data tile
      //    xxnStrings() == returns a NorthernUI strings tile
      //
      // Selectors with tests pending:
      //
      //    descendant(name) == searches the tile's descendants and returns the first found tile with the specified name
      //                         - is this redundant with undocumented functionality in the vanilla child() ?
      //
      // Selectors that cannot work on templated content:
      //
      //    siblingbyindex(123) == selects child #123 of the current tile's parent
      //       In theory, generated tiles would not be able to use this to select next-siblings, 
      //       because those next-siblings do not exist when the generated content goes through 
      //       second-pass parsing.
      //
      // Selectors that are impossible:
      //
      //    menu(menuID) == root tile of the specified menu
      //       Menus can be opened and closed. If a tile from one menu successfully pulls the root 
      //       tile of another menu by way of the SRC system, and then that other menu is closed, 
      //       I don't know what would happen to the trait. Would the pointer be found and cleaned 
      //       up, or would we eventually see memory corruption? Moreover, even if the pointer is 
      //       cleaned up, it wouldn't be reinstated if that other menu were reopened.
      //

      void OnINIChange(NorthernUI::INI::INISetting* s, NorthernUI::INI::ChangeCallbackArg oldVal, NorthernUI::INI::ChangeCallbackArg newVal) {
         if (!g_northernUIDatastore)
            return;
         {  // bShowHUDClock
            auto& desired = NorthernUI::INI::Features::bShowHUDClock;
            if (!s || s == &desired) {
               UInt32 traitID = RE::GetOrCreateTempTagID("_xxnshowhudclock", -1);
               CALL_MEMBER_FN(g_northernUIDatastore, UpdateFloat)(traitID, desired.bCurrent ? RE::kEntityID_true : RE::kEntityID_false);
            }
         }
         {  // bShowHUDInputViewer
            auto& desired = NorthernUI::INI::Features::bShowHUDInputViewer;
            if (!s || s == &desired) {
               UInt32 traitID = RE::GetOrCreateTempTagID("_xxnshowhudinputviewer", -1);
               CALL_MEMBER_FN(g_northernUIDatastore, UpdateFloat)(traitID, desired.bCurrent ? RE::kEntityID_true : RE::kEntityID_false);
            }
         }
         {  // bUsePlaystationButtonIcons
            auto& desired = NorthernUI::INI::Features::bUsePlaystationButtonIcons;
            if (!s || s == &desired) {
               UInt32 traitID = RE::GetOrCreateTempTagID("_xxnuseplaystationbuttonicons", -1);
               CALL_MEMBER_FN(g_northernUIDatastore, UpdateFloat)(traitID, desired.bCurrent ? RE::kEntityID_true : RE::kEntityID_false);
            }
         }
      };
      //
      namespace SetupAndTeardown {
         //
         // Code to handle setting up and tearing down our g_northernUIDatastore tile in synch with 
         // the vanilla screen() and strings() tiles.
         //
         namespace Setup { // Patch InterfaceManager::InitializeMenuRootAndStrings
            constexpr char* pathDatastore = "Data\\Menus\\NorthernUI\\datastore.xml";
            constexpr char* pathStrings   = "Data\\Menus\\NorthernUI\\strings.xml";
            constexpr char* pathLocCfg    = "Data\\Menus\\NorthernUI\\localizationconfig.xml";
            void Inner(RE::Tile* menuRoot) {
               if ((*g_FileFinder)->FindFile(pathLocCfg, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
                  _MESSAGE("XXNLocalization file is missing. The xxnLocalization tile will be nullptr.");
               } else {
                  g_northernUILocConfigTile = CALL_MEMBER_FN(menuRoot, ReadXML)(pathLocCfg);
                  if (g_northernUILocConfigTile)
                     CALL_MEMBER_FN(g_northernUILocConfigTile, AppendToTile)(nullptr, nullptr); // remove from Tile hierarchy
               }
               //
               if ((*g_FileFinder)->FindFile(pathDatastore, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
                  _MESSAGE("Datastore file is missing. The datastore will be nullptr.");
               } else {
                  g_northernUIDatastore = CALL_MEMBER_FN(menuRoot, ReadXML)(pathDatastore);
                  if (g_northernUIDatastore)
                     CALL_MEMBER_FN(g_northernUIDatastore, AppendToTile)(nullptr, nullptr); // remove from Tile hierarchy
               }
               //
               if ((*g_FileFinder)->FindFile(pathStrings, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
                  _MESSAGE("XXNStrings file is missing. The xxnStrings tile will be nullptr.");
               } else {
                  g_northernUIStringstore = CALL_MEMBER_FN(menuRoot, ReadXML)(pathStrings);
                  if (g_northernUIStringstore)
                     CALL_MEMBER_FN(g_northernUIStringstore, AppendToTile)(nullptr, nullptr); // remove from Tile hierarchy
               }
               //
               {  // prefs
                  auto tile = RE::TileRect::CreateOnGameHeap();
                  tile->Unk_01(nullptr, "NorthernUI Pref Storage and Synchronization", nullptr);
                  g_northernUIPrefstore = tile;
                  //
                  UIPrefManager::GetInstance().pushAllPrefsToUIState();
               }
               //
               OnINIChange(nullptr, 0, 0); // apply traits that are drawn from INI settings, for initial load (we'd do this in the outermost Apply() but the tile doesn't exist then!)
               if (g_northernUIDatastore) {  // Apply initial-only traits.
                  UInt32 traitID = RE::GetOrCreateTempTagID("_xxnxinputpatchapplied", -1);
                  CALL_MEMBER_FN(g_northernUIDatastore, UpdateFloat)(traitID, g_xInputPatchApplied ? RE::kEntityID_true : RE::kEntityID_false);
               }
               _MESSAGE("[Selectors] Special tiles have been set up.");
            };
            __declspec(naked) void Outer() { // patches an InterfaceManager member function
               _asm {
                  mov  eax, 0x0058D1C0; // Tile::AppendToTile
                  call eax; // reproduce patched-over call
                  //
                  mov  eax, dword ptr [edi + 0x68]; // eax = this->menuRoot;
                  push eax;
                  call Inner;
                  add  esp, 4;
                  mov  ecx, 0x00581E9C;
                  jmp  ecx;
               };
            };
            void Apply() {
               WriteRelJump(0x00581E97, (UInt32)&Outer);
            };
         };
         namespace Teardown { // Patch InterfaceManager::~InterfaceManager
            void Inner() {
               RE::Tile* t = g_northernUIDatastore;
               if (t) {
                  g_northernUIDatastore = nullptr;
                  t->Dispose(true);
               }
               if (t = g_northernUIStringstore) {
                  g_northernUIStringstore = nullptr;
                  t->Dispose(true);
               }
               if (t = g_northernUILocConfigTile) {
                  g_northernUILocConfigTile = nullptr;
                  t->Dispose(true);
               }
            };
            __declspec(naked) void Outer() {
               _asm {
                  call Inner;
                  mov  ecx, dword ptr [ebp + 0x68];
                  cmp  ecx, ebx;
                  mov  eax, 0x00581B31;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x00581B2C, (UInt32)&Outer);
            };
         };
         void Apply() {
            Teardown::Apply();
            Setup::Apply();
         };
      };
      namespace SrcProcess {
         //
         // Patch for Bethesda's code to recognize tile selectors, to add custom selectors of our own.
         //
         namespace Inner {
            //
            // Selector "inner" functions.
            //
            RE::Tile* Descendant(RE::Tile* basis, const char* arg) {
               if (basis == nullptr)
                  return nullptr; // TODO: Should this search from InterfaceManager::menuRoot instead?
               return RE::GetDescendantTileByName(basis, arg);
            };
            RE::Tile* Menu(RE::Tile* basis, const char* arg) {
               if (basis == nullptr)
                  return nullptr;
               do {
                  if (basis->GetTypeID() == TileMenu::kID)
                     return basis;
               } while (basis = basis->parent);
               return nullptr;
            };
            RE::Tile* NorthernUI(const char* arg) {
               return g_northernUIDatastore;
            };
            RE::Tile* XXNStrings(const char* arg) {
               return g_northernUIStringstore;
            };
            RE::Tile* XXNLocalization(const char* arg) {
               return g_northernUILocConfigTile;
            };
            RE::Tile* XXNPrefs(const char* arg) {
               return g_northernUIPrefstore;
            };
         };
         __declspec(naked) void Outer() {
            //
            // "Outer" function serving as a switchboard, checking the selector's trait ID 
            // and passing the selector argument (and, when needed, the basis tile) to the 
            // "inner" functions, which do specific handling for each custom selector.
            //
            _asm {
               cmp  eax, 0x389; // menu() should return the containing menu
               je   lMenu;
               cmp  eax, CobbPatches::TagIDs::_traitNorthernUI; // northernui() should return our data tile
               je   lNorthernUI;
               cmp  eax, CobbPatches::TagIDs::_traitStrings; // xxnStrings() should return our strings tile
               je   lXXNStrings;
               cmp  eax, CobbPatches::TagIDs::_traitDescendant; // descendant(tilename)
               je   lDescendant;
               cmp  eax, CobbPatches::TagIDs::_traitLocConfig; // xxnLocalization()
               je   lXXNLocalization;
               cmp  eax, CobbPatches::TagIDs::_traitPrefs; // xxnPrefs()
               je   lXXNPrefs;
               jmp  lExitUnhandled;
            lDescendant:
               lea  eax, [esp + 0xC];
               push eax;
               push esi;
               call Inner::Descendant;
               add  esp, 8;
               jmp  lExitHandled;
            lMenu:
               lea  eax, [esp + 0xC];
               push eax;
               push esi;
               call Inner::Menu;
               add  esp, 8;
               jmp  lExitHandled;
            lNorthernUI:
               lea  eax, [esp + 0xC];
               push eax;
               call Inner::NorthernUI;
               add  esp, 4;
               jmp  lExitHandled;
            lXXNStrings:
               lea  eax, [esp + 0xC];
               push eax;
               call Inner::XXNStrings;
               add  esp, 4;
               jmp  lExitHandled;
            lXXNLocalization:
               lea  eax, [esp + 0xC];
               push eax;
               call Inner::XXNLocalization;
               add  esp, 4;
               jmp  lExitHandled;
            lXXNPrefs:
               lea  eax, [esp + 0xC];
               push eax;
               call Inner::XXNPrefs;
               add  esp, 4;
               //
               // When adding new cases, make sure that all but the last end with the 
               // line: jmp lExitHandled;
               //
               // That means the case just above this comment needs that line added if 
               // you slip something new beneath it!
               //
            lExitHandled:
               mov  ecx, 0x0058BA36;
               jmp  ecx;
               //
            lExitUnhandled:
               cmp  eax, 0x1389; // patched-over instruction
               mov  ecx, 0x0058B83E;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x0058B839, (UInt32)&Outer);
         };
      };
      namespace UpdateDatastoreTile {
         //
         // Update the datastore tile (selector northernui()) on every frame, to allow menus to see 
         // certain bits of application state.
         //
         void Inner() {
            auto tile = g_northernUIDatastore;
            if (!tile)
               return;
            {  // _xxnGamepadAvailable
               static UInt32 traitID = RE::GetOrCreateTempTagID("_xxngamepadavailable", -1); // these traits MAY be refcounted, but our tile should never be destroyed until the program exits, so...
               auto  core  = XXNGamepadSupportCore::GetInstance();
               bool  trait = CALL_MEMBER_FN(tile, GetFloatTraitValue)(traitID) == 2.0F;
               if (core->anyConnected != trait)
                  CALL_MEMBER_FN(tile, UpdateFloat)(traitID, core->anyConnected ? 2.0F : 1.0F);
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x005A82D0; //
               call eax;             // replicate patched-over call to UpdateHUDReticleTileOpacities
               call Inner;
               retn;
            };
         };
         void Apply() {
            WriteRelCall(0x00582251, (UInt32)&Outer);
         };
      };
      
      void Apply() {
         SrcProcess::Apply();
         SetupAndTeardown::Apply();
         UpdateDatastoreTile::Apply();
         //
         NorthernUI::INI::RegisterForChanges(&OnINIChange);
         //OnINIChange(nullptr, 0, 0); // has to be done in the Setup hook so that the tile actually exists
      };
   };
};