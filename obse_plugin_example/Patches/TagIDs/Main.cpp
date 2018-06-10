#include "Main.h"
#include "obse_common/SafeWrite.h"
#include "shared.h"

#include "ReverseEngineered/NetImmerse/NiTypes.h"
#include "ReverseEngineered/UI/Tile.h"

#include "Patches/CleanUpAfterMenuQue.h" // needed so we can vary parser behavior for MenuQue
#include "./Traits.h"
#include "./Operators.h"

namespace CobbPatches {
   namespace TagIDs {
      bool IsStringTraitAffectingTextRender(UInt32 id) {
         if (id == RE::kTagID_string)
            return true;
         if (id == _traitFontPath)
            return true;
         return false;
      };
      bool IsOperator(UInt32 id) {
         if (id >= 0x7D1 && id <= 0x7EB)
            return true;
         if (id >= menuQueOperatorMin && id <= menuQueOperatorMax)
            return true;
         for (UInt32 i = 0; i < std::extent<decltype(tagIDs)>::value; i++) {
            const auto& tag = tagIDs[i];
            if (id == tag.id)
               if (tag.type == TagIDType::kType_Operator)
                  return true;
         }
         return false;
      };
      bool IsCustomOperator(UInt32 id) {
         if (id >= 0x7EC && id <= 0x7F0) // MenuQue operators
            return true;
         for (UInt32 i = 0; i < std::extent<decltype(tagIDs)>::value; i++) {
            const auto& tag = tagIDs[i];
            if (id == tag.id)
               if (tag.type == TagIDType::kType_Operator)
                  return true;
         }
         return false;
      };
      bool IsTile(UInt32 id) {
         return (id >= 0x385 && id <= 0x38B);
      }
      bool IsTrait(UInt32 id) {
         if ((id >= 0xFA1 && id <= 0x1001) || id >= 0x2710)
            return true;
         if (id >= menuQueTraitMin && id <= menuQueTraitMax)
            return true;
         for (UInt32 i = 0; i < std::extent<decltype(tagIDs)>::value; i++) {
            const auto& tag = tagIDs[i];
            if (id == tag.id)
               if (tag.type == TagIDType::kType_Trait)
                  return true;
         }
         return false;
      };
      bool IsCustomStringTrait(UInt32 id) {
         for (UInt32 i = 0; i < std::extent<decltype(tagIDs)>::value; i++) {
            const auto& tag = tagIDs[i];
            if (id == tag.id)
               if (tag.type == TagIDType::kType_Trait && tag.isString)
                  return true;
         }
         return false;
      };
      //
      namespace Registration {
         //
         // Register our tag IDs.
         //
         void Inner(const char* vanillaName, SInt32 vanillaCode) {
            _MESSAGE("Registering new traits... The first one will be vanilla, since we patched over a vanilla call to get the timing right.");
            RE::RegisterTrait(vanillaName, vanillaCode); // vanilla call
            //
            for (UInt32 i = 0; i < std::extent<decltype(tagIDs)>::value; i++) {
               const auto& tag = tagIDs[i];
               if (!tag.isMenuQue)
                  RE::RegisterTrait(tag.name, tag.id);
            }
            //
            _MESSAGE("Registered new traits.");
         };
         __declspec(naked) void Outer(const char* vanillaName, SInt32 vanillaCode) {
            _asm {
               mov  eax, dword ptr[esp + 8];
               push eax;
               mov  eax, dword ptr[esp + 8];
               push eax;
               call Inner;
               add  esp, 8; // clean up after non-member call
               retn 8; // replaced call was a member function, so we need to pretend to be
            }
         };
         //
         void Apply() {
            WriteRelCall(0x0058A1CA, (UInt32)&Outer);
         };
      };
      namespace TileTemplate_AddTemplateItem { // we have to replace a vanilla subroutine since MenuQue does it
         //
         // Previously, I simply patched the parts of TileTemplate::AddTemplateItem that checked 
         // tag IDs against valid trait and operator IDs. Unfortunately, however, MenuQue just 
         // replaces that subroutine entirely (as part of the larger, sweeping changes it makes 
         // to the parse process). As such, I can force myself to maintain two completely diff-
         // erent patch methods... or I can just replace the subroutine, blow away MenuQue in 
         // the process, and recreate the changes it makes to the subroutine.
         //
         // The second is more maintainable, so that's what we're going with.
         //
         class Shim : public RE::TileTemplate {
            public:
               void AddTemplateItem(UInt32 aId, const char* aValue, UInt32 aLineNo, bool Arg4) {
                  typedef   RE::Tile::TileTemplateItem::Code   ParseCode;
                  constexpr UInt32 kTileValue_template = 0x3E7;
                  //
                  //auto esp18 = this;
                  float tagIdOrTraitValue = 0.0; // tagIdOrTraitValue
                  if (!Arg4)
                     tagIdOrTraitValue = RE::TagNameToID(aValue);
                  if (!tagIdOrTraitValue && aValue[0] == '_')
                     tagIdOrTraitValue = RE::GetOrCreateTempTagID(aValue, -1);
                  auto tItemNew = RE::Tile::TileTemplateItem::Create(aId, tagIdOrTraitValue, aValue, (SInt32)tagIdOrTraitValue, aLineNo); // ebp
                  {
                     /*// Vanilla code. Should be retained for reference.
                     bool   isNumber = true;
                     UInt32 i = 0; // edx
                     UInt32 L = strlen(aValue);
                     for (UInt32 i = 0; i < L; i++) { // at 0x0058D3CB
                        char c = aValue[i];
                        if ((c < '0' || c > '9') && c != '-' && c != '.') {
                           isNumber = false;
                           break;
                        }
                     }
                     if (isNumber) {
                        if (-1.0 == tagIdOrTraitValue) // at 0x0058D40E
                           sscanf(aValue, "%f", &tagIdOrTraitValue);
                        if (0.0 == tagIdOrTraitValue)
                           sscanf(aValue, "%f", &tagIdOrTraitValue);
                        CALL_MEMBER_FN(&tItemNew->string, Replace_MinBufLen)("", 0);
                        tItemNew->tagType = tagIdOrTraitValue;
                        tItemNew->result = (SInt32)tagIdOrTraitValue;
                     }
                     //*/
                     // Modded code.
                     if (tItemNew->ExtendedStrOrNumParse()) { // is a number
                        //
                        // ExtendedStrOrNumParse already does the job of wiping the string 
                        // and saving the float value, for numeric strings.
                        //
                        tagIdOrTraitValue = tItemNew->tagType;
                     }
                     //*/
                  }
                  //auto edx = this; // at 0x0058D465
                  RE::Tile::TileTemplateItem* tItemZ = nullptr; // ebx // = node[length - 1].data // Z
                  RE::Tile::TileTemplateItem* tItemY = nullptr; // esi // = node[length - 2].data // Y
                  RE::Tile::TileTemplateItem* tItemX = nullptr; // edi // = node[length - 3].data // X
                  {
                     auto eax = this->items.end;
                     if (eax) {
                        tItemZ = eax->data;
                        eax    = eax->prev;
                        if (eax) {
                           tItemY = eax->data;
                           eax    = eax->prev;
                           if (eax)
                              tItemX = eax->data;
                        }
                     }
                  }
                  // at 0x0058D493
                  // ecx == ebx->unk00
                  if (aId == ParseCode::kCode_AttributeName) { // NAME attribute -- tile or template start
                     if (tItemZ && tItemZ->unk00 == ParseCode::kCode_StartTag) {
                        if (tItemZ->tagType == kTileValue_template) {
                           //
                           // TERMINATING BRANCH #0: TEMPLATE START
                           //
                           // tItemZ   == <template... // tagType is the ID for "template"
                           // tItemNew == name="..."   // tagType is the attribute ID (kTileValue_name); string is the value
                           //
                           auto owner = this->owner; // 0x0058D4C9
                           if (owner->unk0C) { // template currently being parsed
                              _MESSAGE("[AddTemplateItem] Can't have nested template definitions in an XML file.");
                           } else {
                              owner->unk0C = CALL_MEMBER_FN(owner, GetTileTemplate)(aValue);
                              if (!owner->unk0C) // at 0x0058D503
                                 owner->unk0C = CALL_MEMBER_FN(owner, AppendTileTemplate)(aValue);
                           }
                           auto item = this->items.RemoveLast(); // removes tItemZ
                           if (item) {
                              item->~TileTemplateItem(); // FormHeap_Free(item->string.m_data);
                              FormHeap_Free(item);       // FormHeap_Free(item);
                           }
                           if (tItemNew) {
                              tItemNew->~TileTemplateItem();
                              FormHeap_Free(tItemNew);
                           }
                           return; // jumps to 0x0058D85F
                        }
                        // jumped to 0x0058D5CD
                        if (tItemZ->unk00 == ParseCode::kCode_StartTag && CobbPatches::TagIDs::IsTile(tItemZ->tagType)) {
                           //
                           // TERMINATING BRANCH #1: TILE START
                           //
                           // tItemZ   == <tagName...  // generic tag-start token; tagType is the tile ID
                           // tItemNew == name="..."   // tagType is the attribute ID (kTileValue_name); string is the value
                           //
                           // Handle this by setting (tItemZ) to a "tile start" token and storing the value of the 
                           // "name" attribute as its (string). Then, destroy (tItemNew) and return.
                           //
                           tItemZ->unk00 = ParseCode::kCode_TileStart;
                           CALL_MEMBER_FN(&tItemZ->string, Replace_MinBufLen)(aValue, 0);
                           if (tItemNew) {
                              tItemNew->~TileTemplateItem();
                              FormHeap_Free(tItemNew);
                           }
                           return; // jumps to 0x0058D943
                        }
                     }
                     // jumped to 0x0058D632
                     //
                     // TERMINATING BRANCH #2
                     //
                     tItemNew->unk00  = ParseCode::kCode_ConstTrait;
                     tItemNew->result = ParseCode::kCode_AttributeName;
                     this->items.Append(tItemNew);
                     return;
                  } else if (g_menuQue.detected) {
                     //
                     // MenuQue compatibility.
                     //
                     if (aId >= ParseCode::kCode_TextContent && aId <= ParseCode::kCode_AttributeMQDest) {
                        auto eax = tItemZ->unk00;
                        if (eax == ParseCode::kCode_TileStart || eax == ParseCode::kCode_TileMQAttribute) {
                           //
                           // MENUQUE TERMINATING BRANCH: NEW ATTRIBUTE
                           //
                           tItemNew->tagType = aId;
                           tItemNew->unk00 = ParseCode::kCode_TileMQAttribute;
                           this->items.Append(tItemNew);
                           return;
                        }
                     }
                     if (tItemZ && tItemZ->unk00 == ParseCode::kCode_MQTextContent
                     && tItemY && (tItemY->unk00 == ParseCode::kCode_NonConstTraitStart || tItemY->unk00 == ParseCode::kCode_NonConstNonSrcOperatorStart)
                     && tItemY->tagType == tagIdOrTraitValue) {
                        //
                        // MENUQUE TERMINATING BRANCH: JUST AFTER TEXTCONTENT OF TRAIT OR OPERATOR
                        //
                        if (CobbPatches::TagIDs::IsTrait(tItemNew->tagType)) {
                           tItemY->unk00 = ParseCode::kCode_ConstTrait;
                        } else if (CobbPatches::TagIDs::IsOperator(tItemNew->tagType)) {
                           tItemY->unk00 = ParseCode::kCode_ConstOperator;
                        } else {
                           tItemY->unk00 = ParseCode::kCode_Invalid;
                           _MESSAGE("[AddTemplateItem:MenuQue-Compat] Bad trait/action type in XML");
                        } // at MQ:0x0001E29D
                        tItemY->result = (SInt32)tItemY->tagType;
                        tItemY->tagType = tItemZ->tagType;
                        CALL_MEMBER_FN(&tItemY->string, Replace_MinBufLen)(tItemZ->string.m_data, 0);
                        if (auto esi = this->items.RemoveLast()) { // removes tItemZ
                           esi->~TileTemplateItem();
                           FormHeap_Free(esi);
                        }
                        tItemNew->~TileTemplateItem();
                        FormHeap_Free(tItemNew);
                        return;
                     }
                  }
                  if (aId == ParseCode::kCode_EndTag) { // at 0x0058D570
                     if (tagIdOrTraitValue == kTileValue_template && this->owner->unk0C) {
                        //
                        // TERMINATING BRANCH #3: TEMPLATE END
                        //
                        this->owner->unk0C = nullptr; // set template currently being parsed to nullptr
                        if (tItemNew) {
                           tItemNew->~TileTemplateItem();
                           FormHeap_Free(tItemNew);
                        }
                        return; // jumps to 0x0058D943
                     }
                     // at 0x0058D66F
                     if (tItemZ) {
                        if (tItemZ->unk00 == ParseCode::kCode_TextContent && tItemY) {
                           if (tItemY->unk00 == ParseCode::kCode_NonConstTraitStart || tItemY->unk00 == ParseCode::kCode_NonConstNonSrcOperatorStart) { // at 0x0058D697
                              if (tItemY->tagType == tagIdOrTraitValue) { // at 0x0058D6A9
                                 //
                                 // TERMINATING BRANCH #4: OPERATOR START OR TRAIT TEXT CONTENT
                                 //
                                 // tItemY   == start of non-const trait
                                 // tItemZ   == tile textContent
                                 // tItemNew == end tag; not template
                                 //
                                 // Handle this by setting tItemY's parse-code to either "trait" or "operator." Then, 
                                 // copy tItemZ's float and string values (tagType and string) to tItemY, and destroy 
                                 // tItemZ and tItemNew.
                                 //
                                 if (CobbPatches::TagIDs::IsTrait(tItemNew->tagType)) {
                                    tItemY->unk00 = ParseCode::kCode_ConstTrait; // near 0x0058D6DA
                                 } else if (CobbPatches::TagIDs::IsOperator(tItemNew->tagType)) { // near 0x0058D6E2
                                    tItemY->unk00 = ParseCode::kCode_ConstOperator;
                                 } else {
                                    tItemY->unk00 = ParseCode::kCode_Invalid;
                                    _MESSAGE("[AddTemplateItem] Bad trait/action type in XML");
                                 }
                                 // at 0x0058D71D
                                 tItemY->result = (SInt32) tItemY->tagType;
                                 tItemY->tagType = tItemZ->tagType;
                                 CALL_MEMBER_FN(&tItemY->string, Replace_MinBufLen)(tItemZ->string.m_data, 0);
                                 auto esi = this->items.RemoveLast(); // removes tItemZ
                                 if (esi) {
                                    esi->~TileTemplateItem();
                                    FormHeap_Free(esi);
                                 }
                                 tItemNew->~TileTemplateItem();
                                 FormHeap_Free(tItemNew);
                                 return; // jumps to 0x0058D943
                              }
                           }
                        }
                        // at 0x0058D78E
                        if (            tItemZ->unk00 == ParseCode::kCode_AttributeTrait
                           && tItemY && tItemY->unk00 == ParseCode::kCode_AttributeSrc
                           && tItemX && tItemX->unk00 == ParseCode::kCode_NonConstNonSrcOperatorStart && tItemX->tagType == tagIdOrTraitValue
                        ) {
                           //
                           // TERMINATING BRANCH #5: OPERATOR SRC/TRAIT CONSTRUCTION
                           //
                           // tItemX == operator start // tagType == operator ID
                           // tItemY == src attribute  // string  == attribute value
                           // tItemZ == trait
                           //
                           tItemX->unk00   = ParseCode::kCode_SrcOperator;
                           tItemX->result  = (SInt32) tItemX->tagType;
                           tItemX->tagType = tItemZ->tagType;
                           CALL_MEMBER_FN(&tItemX->string, CopyValueFromOther)(&tItemY->string);
                           auto esi = this->items.RemoveLast(); // removes tItemZ
                           if (esi) {
                              esi->~TileTemplateItem();
                              FormHeap_Free(esi);
                           }
                           if (esi = this->items.RemoveLast()) { // removes tItemY
                              esi->~TileTemplateItem();
                              FormHeap_Free(esi);
                           }
                           tItemNew->~TileTemplateItem();
                           FormHeap_Free(tItemNew);
                           return; // jumps to 0x0058D943
                        }
                     }
                     ; // just FPU manip // at 0x0058D87D
                  }
                  //
                  // at 0x0058D87F
                  //
                  // TERMINATING BRANCH #6: ALL OTHER CASES
                  //
                  if (CobbPatches::TagIDs::IsTrait(tItemNew->result)) { // at 0x0058D895
                     //
                     // We're currently handling a trait element.
                     //
                     if (tItemNew->unk00 == ParseCode::kCode_StartTag)
                        tItemNew->unk00 = ParseCode::kCode_NonConstTraitStart;
                     else if (tItemNew->unk00 == ParseCode::kCode_EndTag)
                        tItemNew->unk00 = ParseCode::kCode_NonConstTraitEnd;
                        //
                  } else if (CobbPatches::TagIDs::IsOperator(tItemNew->result)) { // at 0x0058D8C2
                     //
                     // We're currently handling an operator element.
                     //
                     if (tItemNew->unk00 == ParseCode::kCode_StartTag)
                        tItemNew->unk00 = ParseCode::kCode_NonConstNonSrcOperatorStart;
                     else if (tItemNew->unk00 == ParseCode::kCode_EndTag)
                        tItemNew->unk00 = ParseCode::kCode_NonConstNonSrcOperatorEnd;
                        //
                  } else if (tItemNew->unk00 == ParseCode::kCode_EndTag) { // at 0x0058D8E7
                     if (CobbPatches::TagIDs::IsTile(tItemNew->tagType))
                        //
                        // We're currently handling the end tag of a tile element.
                        //
                        tItemNew->unk00 = ParseCode::kCode_TileEnd;
                  }
                  // at 0x0058D910
                  this->items.Append(tItemNew); // inlined
                  return; // at 0x0058D943
               }
         };
         __declspec(naked) void Outer() {
            _asm {
               jmp Shim::AddTemplateItem; // route to our shim
            };
         };
         //
         namespace Diagnostics_Incremental {
            //
            // Wraps AddTemplateItem and allows us to see tokens as they're constructed or 
            // converted. Only works with the vanilla subroutine and the MenuQue subroutine.
            //
            // WARNING: THIS WILL SPAM THE LOGS! With my datastore and xxnStrings files in 
            // place and fully written out, I got 100MB of logs before the main menu was even 
            // done loading.
            //
            static UInt32 s_menuQuePatchAddr = 0;
            //
            void Inner(RE::TileTemplate* tmpl) {
               _MESSAGE("Last execution left the template in the following state:", tmpl);
               auto node = tmpl->items.start;
               if (node) {
                  do {
                     auto data = node->data;
                     if (!data)
                        continue;
                     _MESSAGE("   Token: 0x%02X: result (%d == %03X); values (%.f | %s).", data->unk00, data->result, data->result, data->tagType, data->string.m_data);
                  } while (node = node->next);
               } else
                  _MESSAGE("   NO TOKENS.");
               _MESSAGE("   LIST ENDS.");
               if (tmpl->owner)
                  _MESSAGE("   OWNER Unk0C == %08X", tmpl->owner->unk0C);
            };
            __declspec(naked) void __stdcall Shim() {
               _asm {
                  mov  eax, s_menuQuePatchAddr;
                  test eax, eax;
                  jnz  lExit;
                  push -1;
                  push 0x009BF90B;
                  mov  eax, 0x0058D2F7;
               lExit:
                  jmp  eax;
               };
            };
            __declspec(naked) void Outer() {
               _asm {
                  push ebp;
                  mov  ebp, esp;
                  push ecx; // protect
                  mov  edx, dword ptr [ebp + 0x14]; // arg4
                  mov  eax, dword ptr [ebp + 0x10]; // arg3
                  push edx;
                  push eax;
                  mov  edx, dword ptr [ebp + 0xC]; // arg2
                  mov  eax, dword ptr [ebp + 0x8]; // arg1
                  push edx;
                  push eax;
                  call Shim;
                  call Inner; // previously-pushed ecx passed as argument
                  mov  esp, ebp; // cleans up stack for us
                  pop  ebp;
                  retn 0x10;
               };
            };
            void Apply() {
               if (*(UInt8*)(0x0058D2F0) == 0xE9) {
                  s_menuQuePatchAddr = *(UInt32*)(0x0058D2F1);
                  s_menuQuePatchAddr += 5;
                  s_menuQuePatchAddr += 0x0058D2F0;
               }
               WriteRelJump(0x0058D2F0, (UInt32)&Outer);
            };
         };
         namespace Diagnostics_Post {
            //
            // Hooks the end of the parse process and allows us to see the final token list.
            //
            void Inner(RE::TileTemplate* tmpl) {
               if (!tmpl) {
                  _MESSAGE("ParseXML has finished. No result.");
                  return;
               }
               _MESSAGE("ParseXML has finished with template at %08X with name %08X: %s. Final tokens follow:", tmpl, tmpl->name.m_data, tmpl->name.m_data);
               auto node = tmpl->items.start;
               if (node) {
                  do {
                     auto data = node->data;
                     if (!data)
                        continue;
                     _MESSAGE("   Token: 0x%02X: result (%d == %03X); values (%.f | %s).", data->unk00, data->result, data->result, data->tagType, data->string.m_data);
                  } while (node = node->next);
               } else
                  _MESSAGE("   NO TOKENS.");
               _MESSAGE("   LIST ENDS.");
               if (tmpl->owner) {
                  _MESSAGE("   OWNER       == %08X", tmpl->owner);
                  _MESSAGE("   OWNER Unk0C == %08X", tmpl->owner->unk0C);
               }
            };
            __declspec(naked) void Outer() {
               _asm {
                  mov  ecx, dword ptr [esp + 0x24]; // reproduce patched-over opcode
                  push ecx; // protect
                  mov  eax, dword ptr [ecx + 0xC];
                  test eax, eax;
                  jnz  lGoAhead;
                  mov  eax, dword ptr [ecx + 0x0];
               lGoAhead:
                  push eax;
                  call Inner;
                  add  esp, 4;
                  pop  eax; // restore into different register
                  mov  ecx, dword ptr [esp + 0x10C0];
                  mov  edi, 0x0058E23D;
                  jmp  edi;
               };
            };
            void Apply() {
               WriteRelJump(0x0058E232, (UInt32)&Outer);
            };
         };
         //
         void Apply() {
            WriteRelJump(0x0058D2F0, (UInt32)&Outer);
            //Diagnostics_Incremental::Apply();
            //Diagnostics_Post::Apply();
         };
      };

      void Apply() {
         Registration::Apply();
         //
         // Divided into other files:
         //
         Traits::Apply();
         Operators::Apply();
         //
         TileTemplate_AddTemplateItem::Apply(); // needs to overwrite MenuQue's replacer for that subroutine
      };
   };
};