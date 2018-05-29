#include "InventoryMenu.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace InventoryMenu {
      namespace FormType {
         //
         // Give each list item the form-type of the inventory item.
         //
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0058CED0; // reproduce patched-over call to Tile::UpdateString
               call eax;
               mov  eax, dword ptr [esi + 0x8]; // ExtraContainerChanges::EntryData::type
               test eax, eax;
               jz   lSet;
               xor  ecx, ecx;
               mov  cl, byte ptr [eax + 0x4]; // TESForm::typeID
               mov  eax, ecx;
            lSet:
               push eax;
               push 0xFC6; // user24
               mov  ecx, edi;
               mov  eax, 0x0057D300;
               call eax;             // edi->UpdateFloatFromUInt32(0xFC6, eax);
            lExit:
               mov  eax, 0x005AB148;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005AB143, (UInt32)&Outer);
         };
      };
      void Apply() {
         FormType::Apply();
      };
   };
};