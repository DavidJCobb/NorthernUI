#include "ContainerMenu.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace ContainerMenu {
      namespace FormType {
         //
         // Give each list item the form-type of the inventory item.
         //
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0058CED0; // reproduce patched-over call to Tile::UpdateString
               call eax;
               mov  eax, dword ptr [edi + 0x8]; // ExtraContainerChanges::EntryData::type
               test eax, eax;
               jz   lSet;
               xor  ecx, ecx;
               mov  cl, byte ptr [eax + 0x4]; // TESForm::typeID
               mov  eax, ecx;
            lSet:
               push eax;
               push 0xFC6; // user24
               mov  ecx, esi;
               mov  eax, 0x0057D300;
               call eax;             // esi->UpdateFloatFromUInt32(0xFC6, eax);
            lExit:
               mov  eax, 0x005999CC;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005999C7, (UInt32)&Outer);
         };
      };
      namespace GamepadFixes {
         __declspec(naked) void LimitCheckToKeyboard(UInt32 control, UInt32 state) {
            _asm {
               // this == OSInputGlobals*
               mov   eax, dword ptr [esp + 0x4];
               mov   al,  byte ptr [eax + ecx + 0x1B7E]; // eax = this->keyboard[control]
               movzx eax, al;
               mov   edx, dword ptr [esp + 0x8];
               push  edx;
               push  al;
               push  0; // force to keyboard scheme
               mov   eax, 0x00403490; // OSInputGlobals::QueryControlState
               call  eax;
               retn  8;
            };
         };
         void Apply() {
            //
            // ContainerMenu closes itself if the Activate or MenuMode keys are pressed, and 
            // this conflicts with gamepad mappings. Patch the feature to be keyboard-exclusive.
            //
            WriteRelCall(0x0059871F, (UInt32)&LimitCheckToKeyboard); // Activate
            WriteRelCall(0x00598740, (UInt32)&LimitCheckToKeyboard); // MenuMode
         };
      };
      void Apply() {
         FormType::Apply();
         GamepadFixes::Apply();
      };
   };
};