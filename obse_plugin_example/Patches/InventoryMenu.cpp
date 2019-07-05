#include "InventoryMenu.h"
#include "ReverseEngineered/ExtraData/ExtraContainerChanges.h"
#include "ReverseEngineered/Forms/hardcoded.h"
#include "ReverseEngineered/Forms/EffectItem.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/InventoryMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "obse/GameForms.h"
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
      namespace XboxVerb {
         //
         // Show the appropriate verb next to the A button icon when mousing over an item.
         //
         // TODO: Mousing over a category header should show "Sort" as the A-button action. 
         // Our current hook can't do this.
         //
         namespace ItemVerbs {
            void _stdcall Inner(UInt8 formType, TESForm* item, RE::ExtraContainerChanges::EntryData* entry) {
               constexpr char* empty = "";
               const char* verb = empty;
               if (item == *RE::ptrRepairHammerForm) {
                  verb = RE::GMST::sReticleVerbs[18]->s; // Repair
               } else {
                  switch (formType) {
                     case kFormType_Ammo:
                     case kFormType_Armor:
                     case kFormType_Clothing:
                     case kFormType_Light:
                     case kFormType_Spell:
                     case kFormType_Weapon:
                        if (CALL_MEMBER_FN(entry, IsEquippedItem)(0)) {
                           verb = RE::GMST::sReticleVerbs[12]->s; // Unequip
                        } else {
                           verb = RE::GMST::sReticleVerbs[11]->s; // Equip
                        }
                        break;
                     case kFormType_Apparatus:
                        verb = RE::GMST::sReticleVerbs[16]->s; // Brew
                        break;
                     case kFormType_Book:
                        {
                           auto book = OBLIVION_CAST(item, TESForm, TESObjectBOOK);
                           if (!book || !book->enchantable.enchantItem)
                              //
                              // In Oblivion, magic scrolls are TESObjectBOOKs with enchantments; per video of 
                              // Xbox gameplay they should not show ANY button prompt (not even Equip). Don't 
                              // confuse them with "books that are flagged as scrolls," as that flag just refers 
                              // to a "scroll" background image.
                              //
                              verb = RE::GMST::sReticleVerbs[5]->s; // Read
                        }
                        break;
                     case kFormType_Ingredient:
                        verb = RE::GMST::sReticleVerbs[14]->s; // Eat
                        break;
                     case kFormType_AlchemyItem: // potion or poison
                        {
                           auto alch = OBLIVION_CAST(item, TESForm, AlchemyItem);
                           if (alch) {
                              auto magic = (RE::MagicItem*) &alch->magicItem;
                              //
                              verb = RE::GMST::sReticleVerbs[13]->s; // Drink
                              if (CALL_MEMBER_FN(&magic->list, IsPoison)())
                                 verb = RE::GMST::sReticleVerbs[17]->s; // Apply
                           }
                        }
                        break;
                     case kFormType_SoulGem:
                        verb = RE::GMST::sReticleVerbs[15]->s; // Recharge
                        break;
                  }
               }
               if (!verb)
                  verb = empty;
               auto root = g_TileMenuArray->data[RE::InventoryMenu::kID - 0x3E9];
               if (root) {
                  CALL_MEMBER_FN((RE::Tile*)root, UpdateString)(RE::kTagID_user11, verb);
                  CALL_MEMBER_FN((RE::Tile*)root, UpdateFloat) (RE::kTagID_user18, verb != empty ? 2.0F : 1.0F);
               }
            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, dword ptr [ebx + 0x8];
                  xor  edx, edx;
                  mov  dl,  byte ptr [eax + 0x4]; // form type
                  push ebx;
                  push eax;
                  push edx;
                  call Inner; // stdcall
                  mov  eax, dword ptr[ebx + 0x8]; // reproduce patched-over instruction
                  push 0; // reproduce patched-over instruction
                  mov  ecx, 0x005A9E69;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x005A9E64, (UInt32)&Outer);
            }
         }
         namespace OtherVerbs {
            void _stdcall Inner(SInt32 tileID) {
               constexpr char* empty = "";
               const char* verb = empty;
               //
               if (tileID >= 13 && tileID <= 18) { // sort header
                  verb = RE::GMST::sSort->s;
               }
               //
               if (!verb)
                  verb = empty;
               auto root = g_TileMenuArray->data[RE::InventoryMenu::kID - 0x3E9];
               if (root) {
                  CALL_MEMBER_FN((RE::Tile*)root, UpdateString)(RE::kTagID_user11, verb);
                  CALL_MEMBER_FN((RE::Tile*)root, UpdateFloat) (RE::kTagID_user18, verb != empty ? 2.0F : 1.0F);
               }
            }
            __declspec(naked) void Outer() {
               _asm {
                  push ebp;
                  call Inner; // stdcall
                  lea  eax, [ebp - 0xE]; // reproduce patched-over instruction
                  cmp  eax, 4;           // reproduce patched-over instruction
                  mov  ecx, 0x005A9C59;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x005A9C53, (UInt32)&Outer);
               SafeWrite8  (0x005A9C58, 0x90); // courtesy NOP
            }
         }
         void Apply() {
            ItemVerbs::Apply();
            OtherVerbs::Apply();
         }
      }
      void Apply() {
         FormType::Apply();
         XboxVerb::Apply(); // needs testing
      };
   };
};