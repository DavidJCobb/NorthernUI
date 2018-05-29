#include "Fonts.h"
#include "Miscellaneous/strings.h"
#include "Patches/TagIDs/Main.h"
#include "ReverseEngineered/UI/Tile.h"
#include "Services/FontShim.h"
#include "obse_common/SafeWrite.h"

#include "Fun/x86Writer.h"

namespace CobbPatches {
   namespace Fonts {
      typedef RE::FontManager::FontInfo FontInfo;
      //
      namespace Accessors {
         //
         // Patch (almost) every part of the game engine that retrieves a font by 
         // index, and double-check the index. If it is above 0x1F (particularly 
         // if the entire low byte is 0xFF), then it's not a vanilla- or MenuQue- 
         // compatible font index; instead, it is a font index which maps to an 
         // array in our FontShim's list of fonts loaded by path.
         //
         namespace eax {
            __declspec(naked) FontInfo* from_ecx() {
               _asm {
                  push ecx;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  retn;
               }
            };
            __declspec(naked) FontInfo* from_edi() {
               _asm {
                  push edi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  retn;
               }
            };
            __declspec(naked) FontInfo* from_esi() {
               _asm {
                  push esi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  retn;
               }
            };
         }
         namespace ebp {
            __declspec(naked) FontInfo* from_ebp() {
               _asm {
                  push ebp;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ebp, eax;
                  retn;
               }
            }
            __declspec(naked) FontInfo* from_edi() {
               _asm {
                  push edi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ebp, eax;
                  retn;
               }
            }
         }
         namespace ebx {
            __declspec(naked) FontInfo* from_edi() {
               _asm {
                  push edi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ebx, eax;
                  retn;
               }
            }
         }
         namespace ecx {
            __declspec(naked) FontInfo* from_ebp() {
               _asm {
                  push ebp;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ecx, eax;
                  retn;
               }
            }
            __declspec(naked) FontInfo* from_edi() {
               _asm {
                  push edi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ecx, eax;
                  retn;
               }
            }
            __declspec(naked) FontInfo* from_edx() {
               _asm {
                  push edx;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ecx, eax;
                  retn;
               }
            }
            __declspec(naked) FontInfo* from_esi() {
               _asm {
                  push esi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  ecx, eax;
                  retn;
               }
            }
         }
         namespace edx {
            __declspec(naked) FontInfo* from_edi() {
               _asm {
                  push edi;
                  call FontShim::GetFontASMCall;
                  add  esp, 4;
                  mov  edx, eax;
                  retn;
               }
            }
         }

         #ifdef COBB_FONT_PATCH_USE_x86_WRITER
         const UInt32 regEax = 0;
         const UInt32 regEcx = 1;
         const UInt32 regEdx = 2;
         const UInt32 regEbx = 3;
         const UInt32 regEsp = 4;
         const UInt32 regEbp = 5;
         const UInt32 regEsi = 6;
         const UInt32 regEdi = 7;

         void LogDebug(int To, int From) {
            char sTo[4];
            char sFrom[4];
            sTo[0] = sFrom[0] = 'e';
            switch (To) {
               case regEax:
                  sTo[1] = 'a'; sTo[2] = 'x'; break;
               case regEbp:
                  sTo[1] = 'b'; sTo[2] = 'p'; break;
               case regEbx:
                  sTo[1] = 'b'; sTo[2] = 'x'; break;
               case regEcx:
                  sTo[1] = 'c'; sTo[2] = 'x'; break;
               case regEdx:
                  sTo[1] = 'd'; sTo[2] = 'x'; break;
               case regEdi:
                  sTo[1] = 'd'; sTo[2] = 'i'; break;
               case regEsp:
                  sTo[1] = 's'; sTo[2] = 'p'; break;
            }
            switch (From) {
               case regEax:
                  sFrom[1] = 'a'; sFrom[2] = 'x'; break;
               case regEbp:
                  sFrom[1] = 'b'; sFrom[2] = 'p'; break;
               case regEbx:
                  sFrom[1] = 'b'; sFrom[2] = 'x'; break;
               case regEcx:
                  sFrom[1] = 'c'; sFrom[2] = 'x'; break;
               case regEdx:
                  sFrom[1] = 'd'; sFrom[2] = 'x'; break;
               case regEdi:
                  sFrom[1] = 'd'; sFrom[2] = 'i'; break;
               case regEsp:
                  sFrom[1] = 's'; sFrom[2] = 'p'; break;
            }
            sTo[3] = sFrom[3] = '\0';
            _MESSAGE("HOOK: %s from %s.", sTo, sFrom);
         };
         UInt32 MakePatchAtRuntime(UInt32 To, UInt32 From) { // because I can't template or macro the patches without MSVC choking
            bool writeDebugLog = true;
            UInt32 size = 0xB + 1;
            if (writeDebugLog)
               size += 18;
            auto writer = x86Writer(size);
            if (writeDebugLog) {
               writer.PUSH(From);
               writer.PUSH(To);
               writer.CALL((UInt32) &LogDebug);
               writer.ADD (x86Writer::esp, (UInt8) 8);
            }
            writer.PUSH((x86Writer::Register) From);
            writer.CALL((UInt32) &FontShim::GetFontASMCall);
            writer.ADD (x86Writer::esp, (UInt8) 4);
            if (To != x86Writer::eax)
               writer.MOV((x86Writer::Register) To, x86Writer::eax);
            writer.RETN();
            //writer.LogDebug();
            return writer.GetAddress();
         };
         #endif

         __declspec(naked) FontInfo* patch00571AC5() {
            _asm {
               mov  eax, edi;
               dec  eax;
               push eax;
               call FontShim::GetFontASMCall;
               add  esp, 4;
               retn;
            };
         };
         __declspec(naked) FontInfo* patch0057247E() { // use a JMP, not a CALL
            _asm {
               mov  eax, dword ptr [esp + 0x54];
               dec  eax;
               push eax;
               call FontShim::GetFontASMCall;
               add  esp, 4;
               mov  ecx, 0x0057248B;
               jmp  ecx;
            };
         }
         __declspec(naked) FontInfo* patch00577914() { // use a JMP, not a CALL
            _asm {
               mov  ecx, dword ptr [esp + 0x28]; // already zero-indiced
               push ecx;
               call FontShim::GetFontASMCall;
               add  esp, 4;
               mov  ecx, 0x00577919;
               jmp  ecx;
            };
         }
         __declspec(naked) FontInfo* patch005828BB() { // use a JMP, not a CALL
            _asm {
               mov  ecx, dword ptr [esp + 0x2C]; // already decremented to zero-indiced in vanilla code
               push ecx;
               call FontShim::GetFontASMCall;
               add  esp, 4;
               mov  ecx, eax;
               mov  eax, 0x005828C7;
               jmp  eax;
            };
         }
         __declspec(naked) FontInfo* patch0057DE06() {
            _asm {
               mov  ecx, dword ptr [esi + 0x14]; // stored as zero-indiced already; don't decrement
               push ecx;
               call FontShim::GetFontASMCall;
               add  esp, 4;
               retn;
            };
         };
         //
         struct Patch {
            struct Call {
               const UInt32 address = 0; // Address to patch.
               const UInt32 target = 0; // If < 0x100, number of bytes to NOP.
               //
               Call(const UInt32 a, const UInt32 b) : address(a), target(b) {};
            };
            struct Nop {
               const UInt32 address = 0;
               const UInt8  count   = 0;
               //
               Nop(const UInt32 a, const UInt8 b) : address(a), count(b) {};
            };
            //
            const Call call;
            const Nop  nop;
            //
            Patch(const Call a, const Nop b) : call(a), nop(b) {};
         };
         #ifndef COBB_FONT_PATCH_USE_x86_WRITER
         const Patch patches[] = {
            Patch({0x00407C58, (UInt32) &eax::from_esi}, {0x00407C5D, 3}), // Replaces: CALL FontManager_GetSingleton; MOV eax, DWORD PTR DS:[eax + esi*4]
            Patch({0x00571AC5, (UInt32) &patch00571AC5}, {0x00571ACC, 4}),
            Patch({0x00576B5D, (UInt32) &eax::from_esi}, {0x00576B62, 3}),
            Patch({0x00576C81, (UInt32) &ecx::from_edi}, {0x00576C8A, 3}),
            Patch({0x00576FDB, (UInt32) &ecx::from_edi}, {0x00576FE0, 3}),
            Patch({0x0057701E, (UInt32) &ecx::from_ebp}, {0x00577023, 3}),
            Patch({0x0057713B, (UInt32) &edx::from_edi}, {0x00577142, 3}),
            Patch({0x0057717D, (UInt32) &eax::from_edi}, {0x00577182, 3}),
            Patch({0x00577194, (UInt32) &edx::from_edi}, {0x00577199, 3}),
            Patch({0x005778EF, (UInt32) &ebp::from_ebp}, {0x005778F4, 3}),
            Patch({0x00585BC9, (UInt32) &eax::from_edi}, {0x00585BCE, 3}),
            Patch({0x00585D16, (UInt32) &eax::from_esi}, {0x00585D1B, 3}),
            Patch({0x0059247A, (UInt32) &ebp::from_edi}, {0x0059247F, 3}),
            Patch({0x0059D1DE, (UInt32) &ebx::from_edi}, {0x0059D1E3, 3}),
            Patch({0x0059D49D, (UInt32) &ecx::from_edi}, {0x0059D4A2, 3}),
            Patch({0x005F69AB, (UInt32) &ecx::from_esi}, {0x005F69B0, 3}),
         };
         #endif
         #ifdef COBB_FONT_PATCH_USE_x86_WRITER
         const Patch patches[] = {
            Patch({0x00407C58, (UInt32) &eax::from_esi}, {0x00407C5D, 3}), // Replaces: CALL FontManager_GetSingleton; MOV eax, DWORD PTR DS:[eax + esi*4]
            Patch({0x00571AC5, (UInt32) &patch00571AC5}, {0x00571ACC, 4}),
            Patch({0x00576B5D, (UInt32) &eax::from_esi}, {0x00576B62, 3}),
            Patch({0x00576C81, (UInt32) MakePatchAtRuntime(regEcx, regEdi) }, {0x00576C8A, 3}),
            Patch({0x00576FDB, (UInt32) MakePatchAtRuntime(regEcx, regEdi) }, {0x00576FE0, 3}),
            Patch({0x0057701E, (UInt32) MakePatchAtRuntime(regEcx, regEbp) }, {0x00577023, 3}),
            Patch({0x0057713B, (UInt32) MakePatchAtRuntime(regEdx, regEdi) }, {0x00577142, 3}),
            Patch({0x0057717D, (UInt32) &eax::from_edi}, {0x00577182, 3}),
            Patch({0x00577194, (UInt32) MakePatchAtRuntime(regEdx, regEdi) }, {0x00577199, 3}),
            Patch({0x005778EF, (UInt32) MakePatchAtRuntime(regEbp, regEbp) }, {0x005778F4, 3}),
            Patch({0x00585BC9, (UInt32) &eax::from_edi}, {0x00585BCE, 3}),
            Patch({0x00585D16, (UInt32) &eax::from_esi}, {0x00585D1B, 3}),
            Patch({0x0059247A, (UInt32) MakePatchAtRuntime(regEbp, regEdi) }, {0x0059247F, 3}),
            Patch({0x0059D1DE, (UInt32) MakePatchAtRuntime(regEbx, regEdi) }, {0x0059D1E3, 3}),
            Patch({0x0059D49D, (UInt32) MakePatchAtRuntime(regEcx, regEdi) }, {0x0059D4A2, 3}),
            Patch({0x005F69AB, (UInt32) MakePatchAtRuntime(regEcx, regEsi) }, {0x005F69B0, 3}),
         };
         #endif
         void Apply() {
            for (UInt32 i = 0; i < std::extent<decltype(patches)>::value; i++) {
               auto patch = patches[i];
               WriteRelCall(patch.call.address, patch.call.target);
               if (patch.nop.address) {
                  UInt32 a = patch.nop.address;
                  UInt8  c = patch.nop.count;
                  for (UInt8 i = 0; i < c; a++, i++)
                     SafeWrite8(a, 0x90);
               }
            }
            _MESSAGE("Fonts - getters - wrote normal patches.");
            {  // Special-case patch.
               WriteRelJump(0x0057247E, (UInt32) &patch0057247E);
               SafeWrite32 (0x00572483, 0x90909090);
               SafeWrite32 (0x00572487, 0x90909090);
            }
            {  // Special-case patch.
               WriteRelJump(0x00577914, (UInt32) &patch00577914);
               SafeWrite16 (0x00577920, 0x9090);
               SafeWrite8  (0x00577922, 0x90);
            }
            {  // Special-case patch.
               WriteRelJump(0x005828BB, (UInt32) &patch005828BB);
               SafeWrite16 (0x005828C4, 0x9090);
               SafeWrite8  (0x005828C6, 0x90);
            }
            {  // Special-case patch.
               WriteRelCall(0x0057DE06, (UInt32) &patch0057DE06);
               //
               // Write: MOV ECX, EAX; NOP;
               //
               SafeWrite8(0x0057DE2D, 0x8B); // MOV...
               SafeWrite8(0x0057DE2E, 0xC0 | (x86Writer::ecx << 3) | x86Writer::eax); // Opcode second argument type, and arguments (32-bit register; ECX, EAX)
               SafeWrite8(0x0057DE2F, 0x90);
            }
            _MESSAGE("Fonts - getters - wrote special patch.");
         }
      }
      namespace Traits {
         //
         // Patch every part of the game's code that checks a Tile's FONT trait and 
         // retrieves a FontManager::FontInfo based on its value, so that our added 
         // xxnFontPath trait is also checked and evaluated. We also need to patch 
         // TileText::SetTextTraitsToDefault so that resetting a tile to default 
         // also resets the xxnFontPath trait.
         //
         // For the former set of patches, we don't return the FontInfo* itself; 
         // instead, we return a font index, which is intercepted by the "Accessors" 
         // patches above.
         //
         UInt32 GetHash(RE::Tile* tile) {
            const char* string = nullptr;
            if (tile->GetStringValue(CobbPatches::TagIDs::_traitFontPath, &string) && cobb::string_has_content(string)) {
               return FontShim::GetInstance().GetIndex(string);
            }
            float font = 1.0F;
            if (tile->GetFloatValue(0x0FD3, &font))
               return ((SInt32) font) - 1;
            return 0;
         }

         namespace Address00592473 { // TileText::Subroutine00592390. Patched code retrieves the FONT trait, which is immediately passed to a FontInfo accessor.
            const UInt32 target = 0x00592473;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  fstp st; // we patched over a cast from float to double
                  push esi;
                  call GetHash;
                  add  esp, 4;
                  retn;
               }
            }
         }
         namespace Address00592FDB { // AlchemyMenu::Subroutine00592FB0, called by the AM's HandleMouseUp method. Patched code retrieves the FONT trait, which is given to a MenuTextInputState.
            const UInt32 target = 0x00592FDB;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  fstp st; // we patched over a cast from float to double
                  mov  eax, dword ptr[esi + 0x2C];
                  push eax;
                  call GetHash;
                  add  esp, 4;
                  retn;
               }
            }
         }
         namespace Address005A1632 { // Same as above, but for EnchantmentMenu (EnchantmentMenu::Subroutine005A1600).
            const UInt32 target = 0x005A1632;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  fstp st; // we patched over a cast from float to double
                  mov  eax, dword ptr[esi + 0x3C];
                  push eax;
                  call GetHash;
                  add  esp, 4;
                  retn;
               }
            }
         }
         namespace Address005C2750 { // Same as above, but for RaceSexMenu (RaceSexMenu::Subroutine005C2730).
            const UInt32 target = 0x005C2750;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  fstp st; // we patched over a cast from float to double
                  mov  eax, dword ptr[esi + 0x30];
                  push eax;
                  call GetHash;
                  add  esp, 4;
                  retn;
               }
            }
         }
         namespace Address005D40EC { // Same as above, but for SigilStoneMenu (SigilStoneMenu::Subroutine005D40C0).
            const UInt32 target = 0x005D40EC;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  fstp st; // we patched over a cast from float to double
                  mov  eax, dword ptr[esi + 0x30];
                  push eax;
                  call GetHash;
                  add  esp, 4;
                  retn;
               }
            }
         }
         namespace Address005D75BC { // Same as above, but for SpellMakingMenu (SpellMakingMenu::Subroutine005D7590).
            const UInt32 target = 0x005D75BC;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  fstp st; // we patched over a cast from float to double
                  mov  eax, dword ptr[esi + 0x54];
                  push eax;
                  call GetHash;
                  add  esp, 4;
                  retn;
               }
            }
         }
         namespace Address00592275 { // TileText::SetTextTraitsToDefault
            const UInt32 target = 0x00592275;
            const char*  empty  = "";
            __declspec(naked) UInt32 Hook() { // this needs to be jumped to, not called
               _asm {
                  mov  eax, 0x0058CED0; // reproduce patched-over call to Tile::UpdateString
                  call eax;             // 
                  mov  ecx, esi;
                  push empty;
                  push CobbPatches::TagIDs::_traitFontPath;
                  mov  eax, 0x0058CED0; // Tile::UpdateString(CobbPatches::Traits::_traitFontPath, "");
                  call eax;
                  mov  eax, 0x0059227A;
                  jmp  eax;
               }
            }
         }
         //
         namespace MenuTextInputState_SetFontID {
            //
            // Special-case: The game will read FONT traits from tiles and pass them to 
            // MenuTextInputState instances (because those instances need to know the ID 
            // of the font to use for word-wrap checks). However, the instances filter out 
            // IDs that aren't 1, 2, or 3. We need to force them to accept our font hashes.
            //
            const UInt32 target = 0x0057D2E5;
            __declspec(naked) UInt32 Hook() {
               _asm {
                  mov eax, dword ptr [esp + 4];
                  cmp eax, 0x1F; // MenuQue max font
                  jg  lSkipNormalHandling;
                  xor eax, eax;
               lSkipNormalHandling:
                  mov dword ptr [ecx + 0x14], eax;
                  mov eax, 0x0057D2EC;
                  jmp eax;
               }
            }
            void Apply() {
               WriteRelJump(target, (UInt32) &Hook);
               SafeWrite16(target + 5, 0x9090); // NOP
            };
         };
         //
         void Apply() {
            //
            // TODO: We should NOP the GetFloatTraitValue calls in vanilla, as they are 
            // made redundant by our own calls. We just need to make sure that we don't 
            // break the FPU stack if we do that. It's not terribly urgent; these hooks 
            // should only run when text is initially drawn anyway.
            //
            WriteRelCall(Address00592473::target, (UInt32) &Address00592473::Hook);
            WriteRelCall(Address00592FDB::target, (UInt32) &Address00592FDB::Hook);
            WriteRelCall(Address005A1632::target, (UInt32) &Address005A1632::Hook);
            WriteRelCall(Address005C2750::target, (UInt32) &Address005C2750::Hook);
            WriteRelCall(Address005D40EC::target, (UInt32) &Address005D40EC::Hook);
            WriteRelCall(Address005D75BC::target, (UInt32) &Address005D75BC::Hook);
            WriteRelJump(Address00592275::target, (UInt32) &Address00592275::Hook); // don't forget: this needs to be a jump
            MenuTextInputState_SetFontID::Apply();
            _MESSAGE("Fonts - traits - wrote patches.");
         }
      }

      void Apply() {
         Accessors::Apply();
         Traits::Apply();
      }
   }
}