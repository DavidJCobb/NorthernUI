#include "Patches/HUDMainMenu.h"
#include "ReverseEngineered/ExtraData/ExtraContainerChanges.h"
#include "ReverseEngineered/UI/Tile.h"
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

      void Apply() {
         ShowWeaponCharge::Apply();
      };
   };
};