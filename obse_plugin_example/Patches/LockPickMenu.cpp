#include "LockPickMenu.h"
#include "obse_common/SafeWrite.h"

#include "XboxGamepad/Main.h"
#include "XboxGamepad/Patch.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/LockPickMenu.h"

namespace CobbPatches {
   namespace LockPickMenu {
      namespace XInput {
         //
         // Handle special-case functionality for Xbox controller support.
         //
         namespace OnOpen {
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x00585190; // reproduce patched-over call to Menu::EnableMenu
                  call eax;             //
                  mov  CobbPatches::XboxGamepad::g_uiCursorGamepadControlEnabled, 1;
                  mov  eax, 0x005AF939;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005AF934, (UInt32)&Outer);
            };
         };
         namespace OnClose {
            void Patch() {
               RE::SetInterfaceManagerCursorAlpha(0xFF); // overwritten call
               CobbPatches::XboxGamepad::g_uiCursorGamepadControlEnabled = false;
            };
            void Apply() {
               WriteRelCall(0x005AF966, (UInt32)&Patch);
            };
         };
         namespace GamepadMappingFixes {
            //
            // A few issues with LockPickMenu:
            //
            //  - The X and A button mappings are hardcoded into HandleNavigationInput. They 
            //    always map to Auto-Attempt and to a minigame click.
            //
            //  - The A button is mapped incorrectly! It forwards to HandleMouseUp, but there 
            //    IS no mouse-up handler. All click logic is handled in HandleMouseDown.
            //
            //  - Because there is no HandleMouseUp, clicks forwarded from keyboard navigation 
            //    do nothing.
            //
            // Solution:
            //
            //  - Patch in a HandleMouseUp handler that recognizes the exit and auto-attempt 
            //    buttons.
            //
            //  - Have that handler also recognize Tile ID #9001 as a minigame click.
            //
            //  - No-op HandleNavigationInput.
            //
            __declspec(naked) void __stdcall HandleMouseUp(SInt32 id, RE::Tile* tile) {
               _asm {
                  mov eax, dword ptr [esp + 0x4];
                  cmp eax, 2; // exit button
                  je  lForward;
                  cmp eax, 5; // auto-attempt button
                  je  lForward;
                  cmp eax, 9001; // HACK: minigame click button
                  jne lExit;
               lForward:
                  mov eax, dword ptr [ecx];
                  mov eax, dword ptr [eax + 0x8];
                  jmp eax; // return this->HandleMouseDown(id, tile);
               lExit:
                  retn 8;
               };
            };
            void Apply() {
               SafeWrite32(RE::LockPickMenu::kVTBL + 0xC, (UInt32)&HandleMouseUp); // add a HandleMouseUp handler
               //
               // No-op HandleNavigationInput in a manner plainly visible in a disassembler (as a 
               // courtesy to anyone who may reverse-engineer the game with my DLL attached):
               //
               SafeWrite16(0x005B0D20, 0xC032);     // 32 C0: XOR AL, AL
               SafeWrite32(0x005B0D22, 0x900008C2); // C2 08 00: RETN 8 // No-op HandleNavigationInput.
               SafeWrite16(0x005B0D26, 0x9090);     // courtesy NOPs
               SafeWrite16(0x005B0D28, 0x90);       // courtesy NOP
            };
         };
         //
         void Apply() {
            if (!g_xInputPatchApplied)
               return;
            OnOpen::Apply();
            OnClose::Apply();
            GamepadMappingFixes::Apply();
         };
      };

      void Apply() {
         XInput::Apply();
      };
   };
};