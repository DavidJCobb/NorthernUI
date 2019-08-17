#include "CleanUpAfterMenuQue.h"

#include <array>
#include "obse_common/SafeWrite.h"
#include "shared.h"

#include "obse/GameTiles.h"
#include "ReverseEngineered/NetImmerse/NiTypes.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "Services/PatchManagement.h"

#include "Patches/TagIDs/Main.h"

#include "Miscellaneous/strings.h"
#include "Fun/x86Reader.h"

MenuQueState g_menuQue;

namespace CobbPatches {
   namespace CleanUpAfterMenuQue {
      constexpr char* ce_menuQueOBSEName = "OBSE_Kyoma_MenuQue";

      bool _GetMQSubmoduleBaseFromWinAPI() {
         constexpr int LOAD_COUNT = 140;
         HANDLE  processHandle = GetCurrentProcess();
         HMODULE modules[LOAD_COUNT];
         DWORD   bytesNeeded;
         if (!EnumProcessModules(processHandle, modules, sizeof(modules), &bytesNeeded))
            return false;
         bool   overflow = bytesNeeded > (LOAD_COUNT * sizeof(HMODULE));
         UInt32 count = (std::min)(bytesNeeded / sizeof(HMODULE), (UInt32)LOAD_COUNT);
         //
         std::string path;
         for (UInt32 i = 0; i < count; i++) {
            TCHAR szModName[MAX_PATH];
            if (!GetModuleFileNameEx(processHandle, modules[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
               continue;
            path = szModName;
            if (cobb::striendswith(path, "data\\obse\\plugins\\menuque\\submodule.game.dll")) {
               MODULEINFO moduleData;
               if (!GetModuleInformation(processHandle, modules[i], &moduleData, sizeof(moduleData))) {
                  g_menuQue.addrBase = (UInt32)modules[i]; // we still have the module base
                  g_menuQue.addrSize = 0;
                  return true;
               }
               g_menuQue.addrBase = (UInt32)moduleData.lpBaseOfDll;
               g_menuQue.addrSize = moduleData.SizeOfImage;
               return true;
            }
         }
         return false;
      }
      void Identify() {
         if (g_obse->GetPluginLoaded(ce_menuQueOBSEName) && _GetMQSubmoduleBaseFromWinAPI()) {
            g_menuQue.detected = true;
            _MESSAGE(" - MenuQue's submodule base address reads as %08X (size %08X) based on Windows APIs.", g_menuQue.addrBase, g_menuQue.addrSize);
            g_menuQue.obseVersion = (MenuQueState::OBSEVersion) g_obse->GetPluginVersion(ce_menuQueOBSEName);
            _MESSAGE(" - OBSE identifies MenuQue as version %08X.", g_menuQue.obseVersion);
            //
            {  // Test for MenuQue $+14860, a KYO::TileLink member function in v16b:
               UInt32 base = g_menuQue.addrBase;
               UInt32 a = *(UInt32*)(base + 0x00014860 + 0);
               UInt32 b = *(UInt32*)(base + 0x00014860 + 4);
               UInt32 c = *(UInt32*)(base + 0x00014860 + 8);
               if (a == 0x51EC8B55 && b == 0x5308458B && c == 0x4339D98B) {
                  _MESSAGE(" - MenuQue fingerprinted as v16b-like.");
                  g_menuQue.fingerprintedVersion = MenuQueState::kFingerprintedVersion_16b;
               } else {
                  _MESSAGE(" - MenuQue version not identifiable through ASM checks.");
               }
            }
         } else {
            _MESSAGE(" - MenuQue not detected.");
         }
      }
      //
      namespace MenuQueFixes {
         //
         // One of MenuQue's patches crashes when menus with extended IDs are open, 
         // and we can't reasonably stop it from running into that case. The only 
         // option is to patch MenuQue itself.
         //
         // The problem occurs in a hook that MenuQue applies to the interface's 
         // frame handler, to operate on the mouseTarget tile. MenuQue tries to 
         // retrieve a TileLink (a new Tile subclass) and then call a method on 
         // it. If the TileLink is nullptr, MenuQue crashes, because there's no 
         // check for that. The pointer will be null if the mouseTarget belongs to 
         // a menu with an extended ID, or if the mouseTarget belongs to a menu 
         // that is not the InterfaceManager::activeMenu.
         //
         // The solution is to patch this TileLink method to check whether the 
         // this pointer is null.
         //
         /*//
         struct Patch {
            const char* version;
            UInt32 hookTarget; // where does the hook jump to?
            //
            UInt32 patchSubroutine;
            UInt32 patchOffset;
            UInt32 returnFailOffset;
            UInt32 returnPassOffset;
         };
         const Patch knownVersions[] = {
            { "16a", 0x00011A50, 0x000145C0, 0xA, 0xF, 0x18 },
            { "16b", 0x00011E30, 0x00014860, 0xA, 0xF, 0x18 },
         };
         //*/
         namespace Patch_v16b {
            static UInt32 s_mqReturnFail    = 0;
            static UInt32 s_mqReturnProceed = 0;
            __declspec(naked) void Outer() {
               _asm {
                  test ebx, ebx;
                  jz   lFail;
                  cmp  dword ptr [ebx + 0x40], eax; // reproduce patched-over instruction
                  je   lFail;
                  mov  ecx, s_mqReturnProceed;
                  jmp  ecx;
               lFail:
                  mov  eax, s_mqReturnFail;
                  jmp  eax;
               };
            };
            void Apply() {
               s_mqReturnFail    = g_menuQue.addrBase + 0x00014860 + 0x0F;
               s_mqReturnProceed = g_menuQue.addrBase + 0x00014860 + 0x18;
               UInt32 target     = g_menuQue.addrBase + 0x00014860 + 0xA;
               WriteRelJump(target, (UInt32)&Outer);
            }
         }
         UInt32 _getJumpTarget(UInt32 jumpFrom) { // arg should be address of 0xE9 byte
            if (*(UInt8*)jumpFrom != 0xE9)
               return 0;
            return *(UInt32*)(jumpFrom + 1) + (jumpFrom + 1) + sizeof(void*);
         }
         void Apply() {
            constexpr UInt32 ce_hookSite = 0x0058251B;
            //
            if (g_menuQue.fingerprintedVersion == MenuQueState::kFingerprintedVersion_16b) {
               Patch_v16b::Apply();
               return;
            }
            UInt32 jumpAbs = _getJumpTarget(ce_hookSite);
            if (!jumpAbs)
               return;
            _MESSAGE(" - Detected hook to InterfaceManager::Update+0x32B. Checking to see if it's a known MenuQue hook.");
            UInt32 jumpRel = jumpAbs - g_menuQue.addrBase;
            if (jumpAbs >= g_menuQue.addrBase && (!g_menuQue.addrSize || jumpAbs < (g_menuQue.addrBase + g_menuQue.addrSize))) {
               _MESSAGE("    - Not a known MenuQue hook. Jump target read as MQ_%08X (%08X).", jumpRel, jumpAbs);
               g_menuQue.newMenuIDFixFailed = true;
            } else {
               _MESSAGE("    - Not a MenuQue hook. Jump target read as %08X.", jumpAbs);
               //
               // If it's not a MenuQue hook, then we don't need to apply the MenuQue menu ID fix: 
               // the broken code won't run anyway. This is known to occur when Oblivion Reloaded 
               // has its Equipment Mode enabled.
            }
         };
      };

      struct Call {
         UInt32 address = 0;
         UInt8  brokenByteCount = 0;
         //
         Call(UInt32 a, UInt8 b) : address(a), brokenByteCount(b) {};
      };
      const Call calls[] =
         {
            //
            // These should be: the address of the patched in call or jump itself; followed by the 
            // number of NOPs needed after the call or jump.
            //
            {0x00576567,  2}, // FontManager::FontManager
            {0x0057FEF7, 10}, // possibly MenuTextInputState::~MenuTextInputState
            {0x0057FF50,  1}, // MenuTextInputState::HandleKeypress
            {0x00580120,  1}, // MenuTextInputState::Subroutine00580120
            {0x0058251B,  5}, // InterfaceManager::Update // calls MQ_14A90
            {0x00583A4B,  3}, // InterfaceManager::Update
            {0x00583AE3,  3}, // InterfaceManager::Update
            {0x00583B59,  1}, // InterfaceManager::Update
            {0x00583C41,  2}, // InterfaceManager::UpdateAndExpireFadeTimers
            {0x0058430B,  2}, // FadeMenu
            {0x00584960,  1}, // Menu::Subroutine00584960
            {0x00585190,  1}, // Menu::EnableMenu
            {0x005866CC,  1}, // unknown; this is the subroutine that handles "reload [MenuName]" console commands
            {0x00589C27,  1}, // Tile::Value::~Value              // related to operators?
            {0x0058B2FC,  1}, // Tile::HandleTraitChanged
            {0x0058BF92,  1}, // Tile::Value::DoActionEnumeration
            {0x0058BFBC,  1}, // Tile::Value::DoActionEnumeration
            {0x0058C04C,  2}, // Tile::Value::DoActionEnumeration
            {0x0058C33E,  5}, // Tile::Value::DoActionEnumeration
            {0x0058C798,  1}, // Tile::Value::DoActionEnumeration
            {0x0058C7BA,  1}, // Tile::Value::DoActionEnumeration
            {0x0058C85F,  1}, // Tile::Value::DoActionEnumeration
            {0x0058CF66,  1}, // Tile::UpdateTemplatedChildren // apparently does special processing when MenuQue's XML entities for behavior are used
            {0x0058D0F8,  1}, // Tile::UpdateTemplatedChildren
            {0x0058D1AC,  5}, // Tile::UpdateTemplatedChildren
            {0x0058D2F0,  2}, // Struct0058BCD0::AddTemplateItem
            {0x0058DC7C,  1}, // ParseXML
            {0x005A40D0,  1}, // unknown; likely ShowGenericMenu; definitely related to GenericMenu
            {0x005A68FF,  2}, // unknown; related to the Big Four; likely handles the switch to StatsMenu
         };

      void Apply() {
         _MESSAGE("MenuQue checks and handling...");
         Identify();
         if (!g_menuQue.detected) {
            PatchManager::GetInstance().FireEvent(PatchManager::Req::P_MenuQue); // fire any other patches that were waiting on us
            return;
         }
         _MESSAGE("Cleaning up after MenuQue patches... (Some leave broken bytes, which is harmless but will confuse a disassembler.)");
         for (UInt32 i = 0; i < std::extent<decltype(calls)>::value; i++) {
            UInt32 address = calls[i].address;
            char opcode = *(char*) address;
            if (opcode != (char) 0xE8 && opcode != (char) 0xE9) { // comparing without a cast compares as a UInt32, fucking hell
               _MESSAGE(" - MenuQue has not patched address %08X (detected opcode is %02X). Leaving it alone.", address, opcode);
               continue;
            }
            _MESSAGE(" - Writing NOPs near address %08X.", address);
            address += 5;
            UInt8 count = calls[i].brokenByteCount;
            SafeMemset(address, 0x90, count);
         }
         _MESSAGE(" - All known patches have been looked at.");
         //
         // Apply any other fixes.
         //
         MenuQueFixes::Apply();
         PatchManager::GetInstance().FireEvent(PatchManager::Req::P_MenuQue); // fire any other patches that were waiting on us
      }
   }
}

namespace {
   namespace MenuQueReverseEngineered {
      //
      // Information about MenuQue gleaned from disassembly; retained just so 
      // I can make enough sense of what it's doing to investigate conflicts.
      //
      struct UnkMenu { // sizeof == 0x34
         const char* menuName; // 00
         UInt32      menuID;   // 04
         UInt32      unk08;    // 08
         void*       menuVtbl; // 0C
         UInt32 unk10;
         UInt32 unk14;
         void*  unk18; // 18 // stores pointer to Menu::HandleFrame (for subclass) after patching the VTBL
         void*  unk1C; // 1C // subroutine pointer; possibly ShowGenericMenu et. al
         void*  unk20; // 20 // subroutine pointer; for GenericMenu, it's a vanilla no-op
         void*  unk24; // 24 // subroutine pointer; for GenericMenu, it's a vanilla no-op
         void*  unk28; // 28 // stores pointer to Menu::HandleFrameMouseDown (for subclass) after patching the VTBL
         void*  unk2C; // 2C // stores pointer to Menu::HandleKeyboardInput  (for subclass) after patching the VTBL
         void*  unk30; // 30 // Menu*; only defined for HUDMainMenu?

         void UnkApplyPatch(UInt32 unkOffset, UInt32 subroutinePointer);
      };
      //
      // KNOWN MENUQUE PATCH SITES NOT CLEANED UP BY US:
      //  - 0x00403F60 - Patches OSInputGlobals::RebindControl.
      //  - 0x0057CEE0 - Patches the very start of InterfaceMenu::ForceCloseAllActiveMenus.
      //  - 0x00582547 - NOPs several instructions in InterfaceManager::Update, with the apparent intention of allowing tiles without IDs to send mouse-up events.
      //  - 0x0058391E - Patches InterfaceManager::Update, with the apparent intention of forcing code that forwards an exit-button click to LoadgameMenu to send the tile pointer rather than just ID#1 and nullptr.
      //  - 0x00585330 - Patches Menu::~Menu.
      //  - 0x00589636 - Fixes the default message duration for messages generated by QueueUIMessage.
      //  - 0x0058B039 - Patches the very end of Tile::DefineTileValuesAndTraits in order to define MenuQue traits.
      //  - 0x0058B86D - Replaces all behavior for the parent() selector.
      //  - 0x0058C01A - Patches Tile::Value::DoActionEnumeration.
      //  - 0x0058DA77 - Patches Tile::Subroutine0058DA70.
      //  - 0x00590330 - Patches the very start of Tile::CreateTemplatedChildren.
      //  - 0x005910AC - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x005910DC - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x0059110C - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x0059113C - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x00591169 - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x00591196 - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x005911D2 - One of eight patch sites apparently used to allow case-insensitive texture paths in menus.
      //  - 0x005AD853 - One of eight patch sites apparently used to allow case-insensitive texture paths in menus. This appears to be a special case for loading screens.
      //  - 0x00591BAF - Patches TileMenu::~TileMenu.
      //  - 0x00593365 - Patches the subroutine that closes AlchemyMenu    and returns control to the Big Four.
      //  - 0x0059BBE9 - Unknown patch apparently related to the ControlsMenu.
      //  - 0x005CEA14 - Patches the subroutine that closes RechargeMenu   and returns control to the Big Four.
      //  - 0x005D041A - Patches the subroutine that closes RepairMenu     and returns control to the Big Four.
      //  - 0x005D4250 - Patches the subroutine that closes SigilStoneMenu and returns control to the Big Four.
      //  - 0x005FB41C - Patches the subroutine that calls ShowAlchemyMenu.
      //  - 0x005FB586 - Patches the subroutine that calls ShowSigilStoneMenu.
      //  - 0x005FB615 - Patches what appears to be an InventoryMenu handler for opening RechargeMenu from a soul gem.
      //  - 0x005FB6D1 - Patches the subroutine that calls ShowRepairMenuAsSubmenu.
      //  - Parts of the jump table at 0x0058BAB4: cases 0x138B (???) and 0x138F (strings).
      //
   };
}