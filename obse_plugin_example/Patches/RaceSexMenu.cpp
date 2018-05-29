#include "Patches/RaceSexMenu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace RaceSexMenu {
      namespace BlockTabsInCharacterName {
         __declspec(naked) void Outer() {
            _asm {
               cmp  eax, 9;          // if (eax == '\t') {
               je   lSkip;           //
               mov  eax, 0x0057FF50; //    ecx->HandleKeypress(eax); // patched-over call; argument already pushed
               call eax;             //
               jmp  lExit;           // }
            lSkip:
               pop eax;
            lExit:
               mov eax, 0x005C3DF2;
               jmp eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005C3DED, (UInt32)&Outer);
         };
      };
      namespace DontBlankNameOnFocus {
         __declspec(naked) void Initial() {
            _asm {
               mov  eax, 0x004DA2A0; // reproduce patched-over call to TESObjectREFR::GetFullName
               call eax;
               push eax; // protect
               //
               mov  ecx, dword ptr [esi + 0x8EC];
               push eax;
               mov  eax, 0x0057FF20; // MenuTextInputState::SetStringsTo
               call eax;
               //
               pop  eax; // restore
               mov  ecx, 0x005CBC3F;
               jmp  ecx;
            };
         };

         void Apply() {
            SafeWrite32(0x005C2739, 0x90909090);
            SafeWrite32(0x005C273D, 0x90909090);
            SafeWrite16(0x005C2741, 0x9090);
            WriteRelJump(0x005CBC3A, (UInt32)&Initial);
         };
      };
      namespace TagUnlabeledButtons {
         const char* vanillaStringSpaces2x  = (const char*) 0x00A6D2F0;
         const char* vanillaStringSpaces10x = (const char*) 0x00A6D30C;

         void Inner(RE::Tile* target, UInt32 traitID, const char* value) {
            CALL_MEMBER_FN(target, UpdateString)(traitID, value);
            //
            float a = 2.0F;
            if (value == vanillaStringSpaces2x || value == vanillaStringSpaces10x)
               a = 1.0F;
            CALL_MEMBER_FN(target, UpdateFloat)(kTileValue_user24, a);
         };
         __declspec(naked) void Outer(UInt32 traitID, const char* value) {
            _asm {
               mov  eax, dword ptr [esp + 0x4];
               mov  edx, dword ptr [esp + 0x8];
               push edx;
               push eax;
               push ecx;
               call Inner;
               add  esp, 0xC;
               retn 8;
            };
         };
         void Apply() {
            WriteRelCall(0x005C458B, (UInt32)&Outer);
         };
      };

      void Apply() {
         BlockTabsInCharacterName::Apply();
         DontBlankNameOnFocus::Apply();
         TagUnlabeledButtons::Apply();
      };
   };
};