#include "./Main.h"
#include "./Traits.h"
#include "obse_common/SafeWrite.h"
#include "shared.h"

#include "ReverseEngineered/UI/Tile.h"

namespace CobbPatches {
   namespace TagIDs {
      namespace Traits {
         namespace XMLParserHooks {
            namespace IsStringTraitCheck {
               //
               // Hook to the XML parser itself: it needs to know whether a found trait is 
               // a float or a string, so that it uses the right method to pass the found 
               // value into a newly-created Tile::Value instance.
               //
               __declspec(naked) void Outer() {
                  _asm {
                     cmp eax, 0x0FDE; // string
                     je  lIsString;
                     cmp eax, 0x0FE6; // filename
                     je  lIsString;
                     push eax; // protect
                     push eax;
                     call IsCustomStringTrait;
                     add  esp, 4;
                     test al, al;
                     pop  eax; // restore
                     jnz  lIsString;
                  lIsFloat:
                     mov ecx, 0x0058D025;
                     jmp ecx;
                  lIsString:
                     mov ecx, 0x0058D050;
                     jmp ecx;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0058D017, (UInt32)&Outer); // (original: CMP) // Tile::UpdateTemplatedChildren+0xD7
                  SafeWrite16 (0x0058D01C, 0x9090);         // NOP (original: JE SHORT)
                  SafeWrite8  (0x0058D01E, 0x90);           // NOP (original: CMP)
                  SafeWrite32 (0x0058D01F, 0x90909090);     // NOP
                  SafeWrite16 (0x0058D023, 0x9090);         // NOP (original: JE SHORT)
                  _MESSAGE("Traits - hooked: XML parser string-trait-check");
               };
            };
            //
            void Apply() {
               IsStringTraitCheck::Apply();
            };
         };
         namespace OtherHooks {
            namespace TileValueStringCheck { // patch Tile::Value::HasStringValue
               __declspec(naked) void Outer() {
                  _asm {
                     movzx edi, di; // cast to UInt32 for comparisons to added traits, below
                     cmp    di, 0xFE6; // filename // reproduce vanilla
                     je    lIsString;
                     pushad;
                     push  edi;
                     call  IsCustomStringTrait;
                     add   esp, 4;
                     test  al, al;
                     popad;
                     jnz   lIsString;
                  lIsFloat:
                     mov ecx, 0x005897B2;
                     jmp ecx;
                  lIsString:
                     mov ecx, 0x005897B6;
                     jmp ecx;
                  }
               };
               void Apply() {
                  WriteRelJump(0x005897AB, (UInt32)&Outer); // (original: CMP)
                  SafeWrite16 (0x005897B0, 0x9090);         // NOP (original: JE SHORT)
                  _MESSAGE("Traits - hooked: Tile::Value::HasStringValue");
               };
            };
            namespace CopyOperatorHook { // patch Tile::Value::DoActionEnumeration
               //
               // The vanilla code flags text tiles as needing to re-render if a 
               // string trait related to text rendering changes (and in practice, 
               // the only such trait in vanilla is the "string" trait). We patch 
               // it to handle custom string traits that affect text rendering.
               //
               __declspec(naked) void Outer() { // hook for COPY operator on traits that should only belong to TEXT tiles
                  _asm {
                     mov   ax, word ptr [edi + 0x18];
                     movsx eax, ax;
                     //
                     cmp eax, 0x0FDE; // string
                     je  lIsString;
                     //
                     // Don't check for the filename trait; the code immediately after 
                     // 0x0058C3C6 handles that as a special case.
                     //
                     pushad;
                     push eax;
                     call IsStringTraitAffectingTextRender;
                     add  esp, 4;
                     test al, al;
                     popad;
                     jnz  lIsString;
                  lIsFloatOrFilename:
                     mov ecx, 0x0058C3C6;
                     jmp ecx;
                  lIsString:
                     mov ecx, 0x0058C39A;
                     jmp ecx;
                  }
               };
               void Apply() {
                  WriteRelJump(0x0058C392, (UInt32)&Outer); // (original: CMP)
                  SafeWrite8  (0x0058C397, 0x90);   // NOP (original: CMP - part of above)
                  SafeWrite16 (0x0058C398, 0x9090); // NOP (original: JNE SHORT)
                  _MESSAGE("Traits - hooked: Tile::Value::DoActionEnumeration - COPY operator");
               };
            };
            namespace TileHandleTraitChangedHook { // patch Tile::HandleTraitChanged
               __declspec(naked) void Outer() {
                  //
                  // Patch to Tile::HandleTraitChanged, which sets the various 
                  // "dirty" flags on a tile when certain traits are changed.
                  //
                  // The vanilla code flags text tiles as needing to re-render if a 
                  // string trait related to text rendering changes (and in practice, 
                  // the only such trait in vanilla is the "string" trait). We patch 
                  // it to handle custom string traits that affect text rendering.
                  //
                  _asm {
                     cmp esi, 0xFD3;
                     je  lIsText;
                     pushad;
                     push eax;
                     call IsStringTraitAffectingTextRender;
                     add  esp, 4;
                     test al, al;
                     popad;
                     jnz  lIsText;
                  lIsOther:
                     mov ecx, 0x0058B36F;
                     jmp ecx;
                  lIsText:
                     //
                     // This trait is related to text rendering. We need to flag 
                     // the tile's text as "dirty," so that it gets reprocessed.
                     //
                     mov ecx, 0x0058B363;
                     jmp ecx;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0058B35B, (UInt32)&Outer); // (original: CMP)
                  SafeWrite8  (0x0058B360, 0x90);   // NOP (original: CMP)
                  SafeWrite16 (0x0058B361, 0x9090); // NOP (original: JNE SHORT)
               };
            };
            //
            void Apply() {
               TileValueStringCheck::Apply();
               CopyOperatorHook::Apply();
               TileHandleTraitChangedHook::Apply();
            };
         };
         //
         void Apply() {
            XMLParserHooks::Apply();
            OtherHooks::Apply();
         };
      };
   };
};