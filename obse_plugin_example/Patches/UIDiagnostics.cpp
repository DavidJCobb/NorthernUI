#include "UIDiagnostics.h"
#include "obse_common/SafeWrite.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/VideoMenu.h"

#include "obse/GameTiles.h"

namespace CobbPatches {
   namespace UIDiagnostics {
      //
      // This code isn't used. We're retaining it for now in case it comes in handy 
      // again later. Right now, it's all set up for specific traits and tile names 
      // as part of the specific use case it was first written and used for. It 
      // should be possible to modify it to be generically usable, e.g. so that you 
      // could specify tiles to "listen for" in an external file or something. 
      // However, I don't need to do that at present, so I haven't yet.
      //
      namespace Operators {
         class Shim : public RE::Tile::Value {
            public:
               void Log_UpdateInboundReferences() {
                  if (this->id == kTileValue_user1 || this->id == kTileValue_id) {
                     {
                        auto tile = this->owner;
                        auto name = tile->name.m_data;
                        if (!name)
                           return;
                        bool match = true;
                        if (stricmp(name, "option_resolution")) {
                           match = false;
                           for (auto parent = tile->parent; parent; parent = parent->parent) {
                              auto name = parent->name.m_data;
                              if (!name)
                                 return;
                              if (stricmp(name, "option_resolution") == 0) {
                                 match = true;
                                 break;
                              }
                           }
                        }
                        if (!match)
                           return;
                     }
                     _MESSAGE("UpdateInboundReferences called for trait %03X on tile %08X (%s). Trait's current value is %.1f / %s.", this->id, this->owner, this->owner->name.m_data, this->num, this->str.m_data);
                  }
               };
         };
         __declspec(naked) void Outer_LogInboundReferences() {
            _asm {
               push edi;      // reproduce patched-over instruction
               mov  edi, ecx; // reproduce patched-over instruction
               call Shim::Log_UpdateInboundReferences;
               mov  eax, dword ptr [edi];
               mov  ecx, 0x0058BDD5;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x0058BDD0, (UInt32)&Outer_LogInboundReferences);
         };
      };
      namespace DebugKeynav {
         namespace CursorCaller {
            void InnerStart() {
               _MESSAGE("InterfaceManager::Subroutine0057FD60();");
            };
            __declspec(naked) void OuterStart() {
               _asm {
                  pushad;
                  call InnerStart;
                  popad;
                  lea  eax, [esp + 0x8];
                  push eax;
                  mov  eax, 0x0057FD69;
                  jmp  eax;
               };
            };
            void __stdcall InnerGetter(RE::Tile* result, SInt32 num) {
               if (result) {
                  _MESSAGE(" - Subroutine0057FD60's call to Subroutine0057DA90 returned tile %08X (%s) with number %d.", result, result->name.m_data, num);
                  {
                     struct x {
                        DEFINE_MEMBER_FN_LONG(x, GetTopmostMenuByDepth, RE::Menu*, 0x005877D0);
                     };
                     DEFINE_SUBROUTINE(x*, Unknown00B3A708_GetSingleton, 0x00587B20, UInt32 one);
                     auto m = CALL_MEMBER_FN(Unknown00B3A708_GetSingleton(1), GetTopmostMenuByDepth)();
                     if (m) {
                        _MESSAGE("    - The menu for that call would've been %08X (%03X) with status %d.", m, m->GetID(), m->unk24);
                        {
                           auto ui = RE::InterfaceManager::GetInstance();
                           auto root = ui->menuRoot;
                           for (auto node = root->childList.start; node; node = node->next) {
                              auto tile = node->data;
                              if (tile) {
                                 auto menu    = CALL_MEMBER_FN(tile, GetContainingMenu)();
                                 bool visible = (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_visible) != 1.0);
                                 auto state   = menu ? menu->unk24 : 0;
                                 float depth  = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_depth);
                                 _MESSAGE("       - Considered menu %08X (%03X / %s). Menu is %svisible, state %d, at depth %f.", menu, menu->GetID(), tile->name.m_data, (visible ? "" : "in"), state, depth);
                              }
                           }
                        }
                     }
                  }
               } else
                  _MESSAGE(" - Subroutine0057FD60's call to Subroutine0057DA90 returned no tile, with num %d.", num);
            };
            __declspec(naked) void OuterGetter() {
               _asm {
                  pushad;
                  mov  ecx, dword ptr [esp + 0x28];
                  push ecx;
                  push eax;
                  call InnerGetter; // stdcall; pops its own arguments
                  popad; // but this would clean up esp anyway
                  test eax, eax;
                  push 0xFDD;
                  mov  edx, 0x0057FD83;
                  jmp  edx;
               };
            };
            void Apply() {
               WriteRelJump(0x0057FD64, (UInt32)&OuterStart);
               WriteRelJump(0x0057FD7E, (UInt32)&OuterGetter);
            };
         };

         void __stdcall Inner(RE::Tile* target, UInt32 targetTrait, UInt32 sourceTrait) {
            if (target)
               _MESSAGE("InterfaceManager::SendKeynavEventTo(%08X (%s), %03X, %03X);", target, target->name.m_data, targetTrait, sourceTrait);
            else
               _MESSAGE("InterfaceManager::SendKeynavEventTo(nullptr, %03X, %03X);", targetTrait, sourceTrait);
         };
         __declspec(naked) void Outer() {
            _asm {
               xor  ebp, ebp;
               mov  esi, ecx;
               pushad;
               mov  eax, dword ptr [esp + 0x34];
               mov  ecx, dword ptr [esp + 0x38];
               mov  edx, dword ptr [esp + 0x3C];
               push edx;
               push ecx;
               push eax;
               call Inner;
               popad;
               cmp  edi, ebp;
               mov  eax, 0x0057F9FE;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x0057F9F8, (UInt32)&Outer);
            SafeWrite8  (0x0057F9FD, 0x90);
            //
            CursorCaller::Apply();
         };
      };
      namespace DebugVideoMenuStartup {
         void PrintTrait(RE::Tile* tile, UInt32 traitId) {
            _MESSAGE("-----------------------------------------------------------");
            _MESSAGE("%08X Tile* %s", tile, tile->name.m_data);
            auto trait = CALL_MEMBER_FN(tile, GetTrait)(traitId);
            if (!trait) {
               _MESSAGE("   Trait %s (%d) not found.", RE::TagIDToName(traitId), traitId);
               _MESSAGE("-----------------------------------------------------------");
               return;
            }
            _MESSAGE("   %08X Trait %s (%d): num == %.1fF, str == %s", trait, RE::TagIDToName(traitId), traitId, trait->num, (trait->str.m_data ? trait->str.m_data : "nullptr"));
            if (trait->operators) {
               _MESSAGE("      Operators:");
               auto o = trait->operators;
               do {
                  if (o->opcode == 0x65) {
                     _MESSAGE("         %08X Operand %d (%03X); ref == %08X", o, o->opcode, o->opcode, o->operand.ref);
                  } else {
                     _MESSAGE("         %08X Operand %d (%03X); float == %.1fF", o, o->opcode, o->opcode, o->operand.immediate);
                  }
                  if (o->refPrev)
                     _MESSAGE("            refPrev == %08X", o->refPrev);
               } while (o = o->next);
            }
            if (trait->incomingRefs) {
               _MESSAGE("      Operators:");
               auto o = trait->incomingRefs;
               do {
                  if (o->opcode == 0x65) {
                     _MESSAGE("         %08X Operand %d (%03X); ref == %08X", o, o->opcode, o->opcode, o->operand.ref);
                  } else {
                     _MESSAGE("         %08X Operand %d (%03X); float == %.1fF", o, o->opcode, o->opcode, o->operand.immediate);
                  }
               } while (o = o->refNext);
            }
            _MESSAGE("-----------------------------------------------------------");
         };
         bool Inner(RE::VideoMenu* menu) {
            UInt32 i = 0;
            bool result = true;
            do {
               if (!menu->tiles[i]) {
                  _MESSAGE("VideoMenu startup: Missing menu tile ID %d!", i + 1);
                  result = false;
               }
            } while (++i < menu->kTileID_Count);
            {  // custom code for debugging
               auto tile = (RE::Tile*) menu->tile;
               if (tile) {
                  auto t = RE::GetDescendantTileByName(tile, "option_resolution");
                  if (t) {
                     PrintTrait(t, kTileValue_user1);
                     auto u = RE::GetDescendantTileByName(t, "enumpicker");
                     if (u) {
                        PrintTrait(u, kTileValue_user1);
                        //
                        auto v = RE::GetDescendantTileByName(u, "button_left");
                        if (v) {
                           PrintTrait(v, kTileValue_id);
                        }
                     }
                  }
               }
            }
            return result;
         };
         __declspec(naked) void Outer() {
            _asm {
               push esi;
               call Inner;
               add  esp, 4;
               test al, al;
               mov  eax, 0x005DEDC9;
               jnz  lCanSpawnMenu;
               jmp  eax;
            lCanSpawnMenu:
               mov  eax, 0x005DEC43;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005DEC30, (UInt32)&Outer);
         };
      };

      void Apply() {
         _MESSAGE("UIDiagnostics::Apply: No.");
      };
   };
};