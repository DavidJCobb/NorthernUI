#include "Patches/HUDMainMenu.h"

#include "ReverseEngineered/ExtraData/ExtraContainerChanges.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/HUDMainMenu.h"
#include "ReverseEngineered/UI/Menus/MapMenu.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "Patches/XboxGamepad/Main.h"
#include "Patches/XboxGamepad/CustomControls.h"
#include "Services/INISettings.h"
#include "Miscellaneous/math.h";
#include "obse/GameMenus.h";
#include "obse_common/SafeWrite.h";

namespace CobbPatches {
   namespace HUDMainMenu {
      namespace ShowWeaponCharge {
         void Inner(RE::Tile* icon, RE::ExtraContainerChanges::EntryData* weaponEntry) {
            float charge = -1.0F; // charge in the enchantment; if no enchantment/item, use a negative number
            SInt32 uses  =  0;    // whether there's enough charge to actually use the enchantment
            if (weaponEntry) {
               auto             form = OBLIVION_CAST(weaponEntry->type, TESBoundObject, TESEnchantableForm);
               EnchantmentItem* enchant;
               if (form && (enchant = form->enchantItem)) {
                  charge = CALL_MEMBER_FN(weaponEntry, GetCharge)();
                  float cost = enchant->magicItem.list.GetMagickaCost(nullptr);
                  uses = charge / cost;
                  //
                  charge /= (float)form->enchantment; // confusingly named -- this field is actually the max charge
               }
            }
            CALL_MEMBER_FN(icon, UpdateFloat)(kTileValue_user20, charge);
            CALL_MEMBER_FN(icon, UpdateFloat)(kTileValue_user21, uses);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0058CEB0; // Tile::UpdateFloat -- args already pushed
               call eax; // patched-over call
               push esi;
               mov  eax, dword ptr [edi + 0x38];
               push eax;
               call Inner;
               add  esp, 8;
               mov  eax, 0x005A7C13;
               jmp  eax;
            }
         };
         void Apply() {
            WriteRelJump(0x005A7C0E, (UInt32)&Outer);
         };
      };
      namespace XInputPlayerMenuModelRotate {
         void _stdcall Inner(RE::HUDMainMenu* menu) {
            static UInt32 lastUpdate = 0;
            //
            if (CALL_MEMBER_FN(*RE::ptrInterfaceManager, GetTopmostMenuID)() != 1) // Abort if not in a "big four" menu
               return;
            {  // Do not rotate if the MapMenu is open.
               auto root = (RE::Tile*)g_TileMenuArray->data[RE::MapMenu::kID - 0x3E9];
               if (root && CALL_MEMBER_FN(root, NiNodeIsNotCulled)())
                  return;
            }
            XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
            if (!gamepad)
               return;
            bool   swap = XXNGamepadConfigManager::GetInstance().swapSticksMenuMode;
            SInt32 xRaw =  gamepad->GetJoystickAxis(swap ? XXNGamepad::kJoystickAxis_LeftX : XXNGamepad::kJoystickAxis_RightX);
            SInt32 yRaw = -gamepad->GetJoystickAxis(swap ? XXNGamepad::kJoystickAxis_LeftY : XXNGamepad::kJoystickAxis_RightY);
            //
            float frameTime = RE::g_timeInfo->frameTime;
            float speedRot  = (float)NorthernUI::INI::XInput::iPlayerMenuModelJoystickMaxRotateSpeed.iCurrent * frameTime;
            float speedZoom = (float)NorthernUI::INI::XInput::iPlayerMenuModelJoystickMaxZoomSpeed.iCurrent   * frameTime;
            //
            float x = (float)xRaw / 100.0F * speedRot;
            float y = (float)yRaw / 100.0F * speedZoom;
            //
            if (xRaw || yRaw)
               RE::UpdateMainMenuPlayerModelAngle(x, y);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x006ECC80; // reproduce patched-over call to TESObjectREFR::GetParentCell
               call eax;
               push eax; // protect
               push ebp;
               call Inner; // stdcall
               pop  eax; // restore
               mov  ecx, 0x005A6E1E;
               jmp  ecx;
            }
         };
         void Apply() {
            WriteRelJump(0x005A6E19, (UInt32)&Outer);
         };
      };

      void Apply() {
         ShowWeaponCharge::Apply();
         XInputPlayerMenuModelRotate::Apply();
      };
   };
};