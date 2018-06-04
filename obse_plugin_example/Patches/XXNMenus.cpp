#include "XXNMenus.h"
#include "obse_common/SafeWrite.h"

#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameData.h" // FileFinder
#include "obse/GameMenus.h"

#include "NewMenus/XXNHUDClockMenu.h"
#include "NewMenus/XXNHUDDebugMenu.h"
#include "NewMenus/XXNOptionsMenu.h"
#include "NewMenus/XXNControlsMenu.h"
//
#include "NewMenus/XXNAlchemyMenu.h"
#include "NewMenus/XXNSampleInventoryMenu.h"

#include "Patches/Selectors.h"

#include "Services/INISettings.h"
#include "shared.h"

namespace CobbPatches {
   namespace XXNMenus {
	   static bool bPatchedMenuMaxIndex = false;
      static bool bPatchedAlchemyMenu  = false;

      namespace ExpandMenuIndices {
         //
         // Makes menu IDs up to 4E8 usable. IDs currently in use:
         //
         // 41C == XXNHUDDebugMenu
         // 41D == XXNHUDClockMenu
         // 41E == XXNOptionsMenu
         // 41F == XXNControlsMenu
         //
         // For every menu that uses an expanded ID, you will need to add a handler to the 
         // CreateMenu_MenuIDSwitchCasePatch nested namespace. This is because the native 
         // CreateMenu function uses a switch-case to construct menus with the correct size 
         // and call the correct constructor. If your menu doesn't have a case, then it can 
         // still be created and displayed, but functions that create menus will fail to 
         // return it, the menu will fail to initialize properly, and it will not be added 
         // to the global menu tile array.
         //
         constexpr SInt8  originalMenuCount = 0x33;
         constexpr SInt8  expandedMenuCount = 0x7F; // original size was 0x33
         static    UInt32 kRepointedCreateMenuSwitchCaseTable[expandedMenuCount];

         namespace CreateMenu_MenuIDSwitchCasePatch {
            namespace XXNHUDClockMenu {
               ::XXNHUDClockMenu* Create() {
                  return ::XXNHUDClockMenu::Create();
               };
               __declspec(naked) void Outer() {
                  _asm {
                     call Create;
                     mov  ecx, 0x005887A3;
                     jmp  ecx;
                  };
               };
            };
            namespace XXNHUDDebugMenu {
               ::XXNHUDDebugMenu* Create() {
                  return ::XXNHUDDebugMenu::Create();
               };
               __declspec(naked) void Outer() {
                  _asm {
                     call Create;
                     mov  ecx, 0x005887A3;
                     jmp  ecx;
                  };
               };
            };
            namespace XXNOptionsMenu {
               ::XXNOptionsMenu* Create() {
                  return ::XXNOptionsMenu::Create();
               };
               __declspec(naked) void Outer() {
                  _asm {
                     call Create;
                     mov  ecx, 0x005887A3;
                     jmp  ecx;
                  };
               };
            };
            namespace XXNControlsMenu {
               ::XXNControlsMenu* Create() {
                  return ::XXNControlsMenu::Create();
               };
               __declspec(naked) void Outer() {
                  _asm {
                     call Create;
                     mov  ecx, 0x005887A3;
                     jmp  ecx;
                  };
               };
            };

            void Setup() {
               _MESSAGE("%s Patching CreateMenu's switch-case table...", "[Patch][XXNMenus][ExpandMenuIndices]");
               _MESSAGE(" - Repointed switch-case table for CreateMenu is at %08X", kRepointedCreateMenuSwitchCaseTable);
               {  // Handle the "default" case for the switch-case table.
                  UInt8 i = 0;
                  do {
                     kRepointedCreateMenuSwitchCaseTable[i] = 0x00588794;
                  } while (++i < expandedMenuCount);
               }
               _MESSAGE(" - Set all pointers in our repointed table to the default case.");
               memcpy(kRepointedCreateMenuSwitchCaseTable, (void*)0x005887B8, 4 * originalMenuCount);
               _MESSAGE(" - Copied existing pointers from the original switch-case table.");
               //
               SafeWrite8 (0x00587D4B, expandedMenuCount - 1); // jump-if-above bounds check
               SafeWrite32(0x00587D55, (UInt32)kRepointedCreateMenuSwitchCaseTable);
               _MESSAGE(" - Switch-case table patched into place.");
               //
               kRepointedCreateMenuSwitchCaseTable[::XXNHUDClockMenu::kID - kMenuType_Message] = (UInt32)&XXNHUDClockMenu::Outer;
               kRepointedCreateMenuSwitchCaseTable[::XXNHUDDebugMenu::kID - kMenuType_Message] = (UInt32)&XXNHUDDebugMenu::Outer;
               kRepointedCreateMenuSwitchCaseTable[::XXNOptionsMenu::kID  - kMenuType_Message] = (UInt32)&XXNOptionsMenu::Outer;
               kRepointedCreateMenuSwitchCaseTable[::XXNControlsMenu::kID - kMenuType_Message] = (UInt32)&XXNControlsMenu::Outer;
               //
               _MESSAGE(" - Menus with expanded IDs have been written into the repointed table.");
            };
         };

         void Apply() {
            _MESSAGE("%s We are going to expand the menu indices to permit %d menu types (max ID %04X).", "[Patch][XXNMenus][ExpandMenuIndices]", expandedMenuCount, kMenuType_Message + expandedMenuCount - 1);
            _MESSAGE("%s Patching g_TileMenuArray's capacity setters...", "[Patch][XXNMenus][ExpandMenuIndices]");
            SafeWrite8(0x00587B6E, expandedMenuCount); // capacity setter for g_TileMenuArray
            SafeWrite8(0x00587B7E, expandedMenuCount); // capacity setter for g_TileMenuArray
            //g_TileMenuArray->SetCapacity(expandedMenuSize); // not needed (and will CTD): array doesn't exist yet
            CreateMenu_MenuIDSwitchCasePatch::Setup();
            bPatchedMenuMaxIndex = true;
         };
      };
      namespace NewHUDWidgets {
         //
         // WARNING: New widgets must double-check that their file exists before 
         // calling ParseXML. ParseXML crashes on missing files! If the file is 
         // absent, returning nullptr instead of a tile should be fine.
         //
         const UInt16 menuIDs[] = { // specify IDs of our extra HUD menus
            XXNHUDClockMenu::kID,
         };
         const UInt16 traitIDs[] = { // specify traits in each menu to set; indices here should match with menuIDs[].
            XXNHUDClockMenu::kMenuModeBoolTrait,
         };
         static_assert(std::extent<decltype(menuIDs)>::value == std::extent<decltype(traitIDs)>::value, "You need to specify the IDs of all menus, and the IDs of the trait to set for each menu. A menu's index in one list should be consistent with its trait's index in the other.");
         //
         namespace Open {
            void Inner() {
               RE::TileMenu* tile;
               //
               // Open each menu in sequence. If it opens successfully, fade the Menu 
               // out (because these will open at the main menu, and potentially render 
               // on top of it).
               //
               if (tile = (RE::TileMenu*) ShowXXNHUDClockMenu()) {
                  auto menu = tile->menu;
                  if (menu)
                     CALL_MEMBER_FN((RE::Menu*)menu, FadeOut)();
               }
            };
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x00588BD0; // reproduce patched-over call
                  call eax;
                  call Inner;
                  mov  eax, 0x005A6C13;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005A6C0E, (UInt32)&Outer);
            };
         };
         namespace Visibility {
            //
            // The subroutine at 005A6040 is responsible for handing HUDMainMenu's visibility 
            // and awareness of MenuMode. When menus other than the "big four" are opened or 
            // closed, this subroutine will forcibly fade HUDMainMenu in and out. Separately 
            // from this, it will set the user3 trait on HUDMainMenu to 0 or 1 depending on 
            // whether certain menus are open (PauseMenu doesn't change the trait; others may 
            // not either).
            //
            // As such, if we want to reliably have our extra HUD menus handle visibility 
            // consistently with the HUDMainMenu, then we need to hook every fade call and 
            // every user3 setter in 005A6040. We'll forcibly fade our menus consistently with 
            // HUDMainMenu, and we'll supply a trait (the menus themselves can choose which) 
            // as well. Our trait won't use the same values; vanilla uses 0 or 1 to indicate 
            // that the game is or isn't in menu mode; we'll use 2 or 1 (&true; or &false;) 
            // instead, consistent with Oblivion XML bools.
            //
            void InnerFade(bool fadeOut) {
               //
               // Don't get this mixed up: passing 1 HIDES the menus, and passing 0 
               // SHOWS them. Yeah, that's backwards from usual, but there are *some* 
               // benefits on the x86 end.
               //
               for (UInt32 i = 0; i < std::extent<decltype(menuIDs)>::value; i++) {
                  auto tile = (RE::TileMenu*) g_TileMenuArray->data[menuIDs[i] - kMenuType_Message];
                  if (!tile)
                     continue;
                  auto menu = tile->menu;
                  if (!menu)
                     continue;
                  if (fadeOut)
                     CALL_MEMBER_FN((RE::Menu*)menu, FadeOut)();
                  else
                     CALL_MEMBER_FN((RE::Menu*)menu, FadeIn)();
               }
            };
            void InnerTrait(float value) {
               if (value == 0.0F)
                  value = 2.0F;
               else
                  value = 1.0F;
               for (UInt32 i = 0; i < std::extent<decltype(menuIDs)>::value; i++) {
                  auto tile = g_TileMenuArray->data[menuIDs[i] - kMenuType_Message];
                  if (!tile)
                     continue;
                  CALL_MEMBER_FN((RE::Tile*)tile, UpdateFloat)(traitIDs[i], value);
               }
            };
            __declspec(naked) void OuterFadeIn() {
               _asm {
                  mov  eax, 0x00584390; // original call, shimmed
                  call eax;
                  push 0;
                  call InnerFade;
                  add  esp, 4;
                  retn;
               };
            };
            __declspec(naked) void OuterFadeOut() {
               _asm {
                  mov  eax, 0x00584740; // original call, shimmed
                  call eax;
                  push 1;
                  call InnerFade;
                  add  esp, 4;
                  retn;
               };
            };
            __declspec(naked) void OuterTrait(UInt32 traitID, float traitValue) {
               _asm {
                  push ebx;
                  push esi;
                  mov  ebx, dword ptr [esp + 0x10];
                  mov  esi, dword ptr [esp + 0xC];
                  push ebx;
                  push esi;
                  mov  eax, 0x0058CEB0; // original call, shimmed
                  call eax;
                  pop  esi; // no longer needed
                  push ebx;
                  call InnerTrait;
                  add  esp, 4;
                  pop  ebx; // no longer needed
                  retn 8;
               };
            };

            __declspec(naked) void PossibleLoadScreenHook(bool show) {
               _asm {
                  //
                  // Reproduce shimmed call:
                  //
                  mov  eax, dword ptr [esp + 0x4];
                  push eax;
                  mov  eax, 0x0057D940; // InterfaceManager::SetHUDReticleVisibility
                  call eax;
                  mov  eax, dword ptr [esp + 0x4];
                  test al, al;
                  setz al;
                  push al;
                  call InnerFade;
                  add  esp, 4;
                  retn 4;
               };
            };

            void Apply() {
               WriteRelCall(0x005A6162, (UInt32)&OuterTrait);
               WriteRelCall(0x005A616C, (UInt32)&OuterFadeOut);
               WriteRelCall(0x005A617F, (UInt32)&OuterTrait);
               WriteRelCall(0x005A6195, (UInt32)&OuterTrait);
               WriteRelJump(0x005A61B2, (UInt32)&OuterFadeIn); // this NEEDS to be a jump, NOT a call
               WriteRelCall(0x005A61C5, (UInt32)&OuterTrait);
               WriteRelCall(0x005A61F8, (UInt32)&OuterFadeIn);
               //
               WriteRelCall(0x005A8748, (UInt32)&OuterTrait); // hide when MainMenu is open
               WriteRelCall(0x00583ED2, (UInt32)&PossibleLoadScreenHook); // pre-load-screen?
               WriteRelCall(0x005842B7, (UInt32)&PossibleLoadScreenHook); // post-load-screen?
            };
         };
         //
         void Apply() {
            if (!bPatchedMenuMaxIndex)
               return;
            Open::Apply();
            Visibility::Apply();
         };
      };

      namespace XXNAlchemyMenu {
         namespace CookPotion {
            void Patch() {
               SafeMemset(0x00594CA0, 0x90, 0x004D);
               SafeMemset(0x00594CFF, 0x90, 0x01F8);
               _MESSAGE("XXNMenus - nuked vanilla AlchemyMenu::CookPotion; we can now call it on a dummy struct to \"feed\" the OBSE hook.");
            };
         };

         const UInt32 pCallShowMenu    = 0x005FB42D;
         const UInt32 pReplaceShowMenu = (UInt32) &ShowXXNAlchemyMenu;
         const UInt32 pAllocationSize  = 0x0058864F + 1;
         const UInt32 pCallConstructor = 0x00588672;

         // This is hideous, but we have to do it like this AND we can't template it because:
         //
         //  - MSVC chokes on templated naked functions
         //  - MSVC chokes on naked static member functions even though they have no "this"
         //  - C++ doesn't let us take the raw address of any member function
         //  - C++ doesn't let us take the raw address of a constructor
         //
         void* ConstructorShimInner(void* addr) {
            new (addr) ::XXNAlchemyMenu();
            return addr;
         };
         __declspec(naked) void ConstructorShimOuter() {
            __asm {
               push ecx;
               call ConstructorShimInner;
               add  esp, 4;
               retn;
            };
         };

         void Apply() {
            if (bPatchedAlchemyMenu)
               return;
            if (!NorthernUI::INI::Menus::bUseXXNAlchemyMenu.bCurrent)
               return;
            if ((*g_FileFinder)->FindFile(::XXNAlchemyMenu::menuPath, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
               _MESSAGE("[XXNMenus] The XML file for the XXNAlchemyMenu is missing. The menu will not be patched in.");
               return;
            }
            WriteRelCall(pCallShowMenu,    pReplaceShowMenu);
            SafeWrite32 (pAllocationSize,  sizeof(::XXNAlchemyMenu));
            WriteRelCall(pCallConstructor, (UInt32) &ConstructorShimOuter);
            _MESSAGE("XXNMenus - patched XXNAlchemyMenu in");
            //
            CookPotion::Patch();
         };
      };
      namespace XXNSampleInventoryMenu {
         const UInt32 pCallShowMenu    = 0x005FB42D;
         const UInt32 pReplaceShowMenu = (UInt32) &::ShowXXNSampleInventoryMenuInsteadOfAlchemyMenu;
         const UInt32 pAllocationSize  = 0x0058864F + 1;
         const UInt32 pCallConstructor = 0x00588672;

         void* ConstructorShimInner(void* addr) {
            new (addr) ::XXNSampleInventoryMenu();
            _MESSAGE("%s: Constructed XXNSampleInventoryMenu at 0x%08X.", __FUNCTION__, addr);
            return addr;
         };
         __declspec(naked) void ConstructorShimOuter() {
            __asm {
               push ecx;
               call ConstructorShimInner;
               add  esp, 4;
               retn;
            };
         };

         void Apply() {
            if (bPatchedAlchemyMenu)
               return;
            if (!NorthernUI::INI::Menus::bUseXXNSampleInventoryMenu.bCurrent)
               return;
            if ((*g_FileFinder)->FindFile(::XXNSampleInventoryMenu::menuPath, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
               _MESSAGE("[XXNMenus] The XML file for the XXNSampleInventoryMenu is missing. The menu will not be patched in.");
               return;
            }
            WriteRelCall(pCallShowMenu, pReplaceShowMenu);
            SafeWrite32(pAllocationSize, sizeof(::XXNSampleInventoryMenu));
            WriteRelCall(pCallConstructor, (UInt32) &ConstructorShimOuter);
            bPatchedAlchemyMenu = true;
            _MESSAGE("XXNMenus - patched XXNSampleInventoryMenu in");
         };
      };

      void Apply() {
         ExpandMenuIndices::Apply();
         NewHUDWidgets::Apply();
         //
         XXNAlchemyMenu::Apply();
         XXNSampleInventoryMenu::Apply();
      };
   }
}
