#include "Exploratory.h"
#include "shared.h"
#include "obse_common/SafeWrite.h"

#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "ReverseEngineered/INISettings.h"

#include "ReverseEngineered/UI/Tile.h"
#include "Services/TileDump.h"

namespace CobbPatches {
   namespace Exploratory {
      namespace EffectSettingMenuGlitchInvestigation {
         void _stdcall Inner(RE::Tile* menuRoot) {
            _MESSAGE("EffectSettingMenu is about to display...");
            auto tile = RE::GetDescendantTileByName(menuRoot, "magnitude_max_prefix");
            if (tile) {
               _MESSAGE("Dumping <magnitude_max_prefix>...");
               TileDump(tile);
            } else
               _MESSAGE("Unable to find <magnitude_max_prefix>.");
            //
            tile = RE::GetDescendantTileByName(menuRoot, "magnitude_max");
            if (tile) {
               _MESSAGE("Dumping <magnitude_max>...");
               TileDump(tile);
            } else
               _MESSAGE("Unable to find <magnitude_max>.");
            //
            tile = RE::GetDescendantTileByName(menuRoot, "magnitude_max_suffix");
            if (tile) {
               _MESSAGE("Dumping <magnitude_max_suffix>...");
               TileDump(tile);
            } else
               _MESSAGE("Unable to find <magnitude_max_suffix>.");
         }
         __declspec(naked) void Outer() {
            _asm {
               push ebx;
               call Inner; // stdcall
               mov  eax, 0x00588BD0; // reproduce patched-over call to Tile::GetFloatTraitValue
               call eax;             //
               mov  ecx, 0x005A09A5;
               jmp  ecx;
            }
         }
         //
         bool _stdcall InnerParse(RE::Tile::Value* value, RE::Tile* arg1, UInt32 arg2, UInt32 arg3) {
            if (value->id == RE::kTagID_x) {
               auto tile = value->owner;
               if (tile && tile->name.m_data && strcmp(tile->name.m_data, "magnitude_max_suffix") == 0) {
                  _MESSAGE("Intercepted append operator to <magnitude_max_suffix>:x... Args are: %08X, %08X, %08X", arg1, arg2, arg3);
                  CALL_MEMBER_FN(value, AppendSrcOperator)(arg1, arg2, arg3);
                  _MESSAGE("Manually appended the new operator. Dumping tile...");
                  TileDump(tile);
                  _MESSAGE("Dump complete.");
                  return true;
               }
            }
            return false;
         }
         __declspec(naked) void OuterParse() {
            _asm {
               push ecx; // protect
               mov  eax, dword ptr [esp + 0xC];
               mov  edx, dword ptr [esp + 0x8];
               push eax;
               push edx;
               mov  eax, dword ptr [esp + 0xC];
               push eax;
               push ecx;
               call InnerParse; // stdcall
               pop  ecx; // restore
               test al, al;
               jnz  lSkip;
               mov  eax, 0x0058CC60; // reproduce patched-over call to Tile::Value::AppendSrcOperator
               call eax;
               jmp  lExit;
            lSkip:
               add  esp, 0xC; // remove args from skipped call
            lExit:
               mov  ecx, 0x0058CF30;
               jmp  ecx;
            }
         }
         //
         namespace UpdateTemplatedChildrenDoActionEnumeration {
            void _stdcall Inner(RE::Tile* subject, RE::Tile::Value* updating) {
               if (!subject->name.m_data)
                  return;
               if (strcmp(subject->name.m_data, "magnitude_max_suffix") != 0)
                  return;
               _MESSAGE("[][][] UpdateTemplatedChildren -> DoActionEnumeration");
               _MESSAGE("[][][] Trait is %s.", RE::TagIDToName(updating->id));
               TileDump(subject);
               _MESSAGE("[][][] Logged iteration.");
            }
            __declspec(naked) void Outer() {
               _asm {
                  push ecx; // protect
                  push ecx;
                  push ebp;
                  call Inner; // stdcall
                  pop  ecx;
                  mov  eax, 0x0058BEE0;
                  call eax;
                  mov  eax, 0x0058CFC7;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x0058CFC2, (UInt32)&Outer);
            }
         }
         namespace DoActionEnumerationUpdateInboundReferences {
            void _stdcall Inner(RE::Tile::Value* updating) {
               if (updating->id != RE::kTagID_x)
                  return;
               RE::Tile* subject = updating->owner;
               if (!subject->name.m_data)
                  return;
               if (strcmp(subject->name.m_data, "magnitude_max_suffix") != 0)
                  return;
               _MESSAGE("[][][] DoActionEnumeration -> UpdateInboundReferences");
               _MESSAGE("[][][] Trait is %s.", RE::TagIDToName(updating->id));
               TileDump(subject);
               _MESSAGE("[][][] Logged iteration.");
            }
            __declspec(naked) void Outer() {
               _asm {
                  push ecx; // protect
                  push esi;
                  call Inner; // stdcall
                  pop  ecx;
                  mov  eax, 0x0058BDD0;
                  call eax;
                  mov  eax, 0x0058C8FF;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x0058C8FA, (UInt32)&Outer);
            }
         }
         namespace DoActionEnumerationHandleTraitChanged {
            void _stdcall Inner(RE::Tile::Value* updating) {
               if (updating->id != RE::kTagID_x)
                  return;
               RE::Tile* subject = updating->owner;
               if (!subject->name.m_data)
                  return;
               if (strcmp(subject->name.m_data, "magnitude_max_suffix") != 0)
                  return;
               _MESSAGE("[][][] DoActionEnumeration -> HandleTraitChanged");
               _MESSAGE("[][][] Trait is %s.", RE::TagIDToName(updating->id));
               TileDump(subject);
               _MESSAGE("[][][] Logged iteration.");
            }
            __declspec(naked) void Outer() {
               _asm {
                  push ecx; // protect
                  push esi;
                  call Inner; // stdcall
                  pop  ecx;
                  mov  eax, 0x0058B2F0;
                  call eax;
                  mov  eax, 0x0058C93B;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x0058C936, (UInt32)&Outer);
            }
         }
         namespace PostUpdateTemplatedChildren {
            void _stdcall Inner(RE::Tile* subject) {
               _MESSAGE("EffectSettingMenu is about to display...");
               auto tile = RE::GetDescendantTileByName(subject, "magnitude_max_suffix");
               if (tile) {
                  _MESSAGE("Dumping <magnitude_max_suffix>: IMMEDIATELY AFTER UPDATETEMPLATEDCHILDREN...");
                  TileDump(tile);
               }

            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x0058CF40; // reproduce patched-over call to Tile::UpdateTemplatedChildren
                  call eax;             //
                  push ebp;
                  call Inner; // stdcall
                  mov  ecx, 0x005904A3;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x0059049E, (UInt32)&Outer);
            }
         }
         //
         void Apply() {
            WriteRelJump(0x005A09A0, (UInt32)&Outer); // ShowEffectSettingMenu+0xC0
            WriteRelJump(0x0058CF2B, (UInt32)&OuterParse);
            //PostUpdateTemplatedChildren::Apply();
            UpdateTemplatedChildrenDoActionEnumeration::Apply();
            DoActionEnumerationUpdateInboundReferences::Apply();
            DoActionEnumerationHandleTraitChanged::Apply();
         }
      }
      namespace Full360AnimsTest {
         void _stdcall Inner() {
         }
         __declspec(naked) void Outer() {
            _asm {
            }
         }
         void Apply() {
         }
      };
      //
      void Apply() {
         Full360AnimsTest::Apply();
         //EffectSettingMenuGlitchInvestigation::Apply();
      }
   }
}