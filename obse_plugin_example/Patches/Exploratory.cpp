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
      /*// TODO: Move this to a debug file.
      namespace TagParseDebug {
         namespace LogFilename {
            void _stdcall Inner(const char* fn) {
               _MESSAGE("===== Loading tile content from: %s", fn);
            }
            __declspec(naked) void Outer() {
               _asm {
                  push edx;
                  call Inner; // stdcall
                  mov  eax, 0x0058DBD0; // reproduce patched-over call to ParseXML
                  call eax;             //
                  mov  ecx, 0x00590456;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x00590451, (UInt32)&Outer);
            }
         }
         namespace LogStage1 {
            void _stdcall Inner(RE::TileTemplate* tmpl) {
               typedef RE::Tile::TileTemplateItem TileTemplateItem;
               //
               _MESSAGE("STAGE 1 PARSE RESULTS:");
               for (auto node = tmpl->items.start; node; node = node->next) {
                  TileTemplateItem* token = node->data;
                  if (!token) {
                     _MESSAGE(" - MISSING TOKEN");
                     continue;
                  }
                  auto line = token->lineNumber;
                  auto str  = token->string.m_data;
                  auto name = RE::TagIDToName(token->result);
                  switch (token->unk00) {
                     case TileTemplateItem::kCode_StartTag:
                        _MESSAGE("Line %03d: START TAG | %08X | %f / %s", line, token->result, token->tagType, str);
                        break;
                     case TileTemplateItem::kCode_EndTag:
                        _MESSAGE("Line %03d: END   TAG | %08X | %f / %s", line, token->result, token->tagType, str);
                        break;
                     case TileTemplateItem::kCode_NonConstNonSrcOperatorStart:
                        if (str)
                           _MESSAGE("Line %03d: <%s> <!-- container operator start; operator name should be %s -->", line, name, str);
                        else
                           _MESSAGE("Line %03d: <%s> <!-- container operator start -->", line, name);
                        break;
                     case TileTemplateItem::kCode_NonConstNonSrcOperatorEnd:
                        if (str)
                           _MESSAGE("Line %03d: <%s> <!-- container operator end; operator name should be %s -->", line, name, str);
                        else
                           _MESSAGE("Line %03d: <%s> <!-- container operator end -->", line, name);
                        break;
                     case TileTemplateItem::kCode_NonConstTraitStart:
                        if (str)
                           _MESSAGE("Line %03d: <%s> <!-- trait start; trait name should be %s -->", line, name, str);
                        else
                           _MESSAGE("Line %03d: <%s> <!-- trait start -->", line, name);
                        break;
                     case TileTemplateItem::kCode_NonConstTraitEnd:
                        if (str)
                           _MESSAGE("Line %03d: </%s> <!-- trait end; trait name should be %s -->", line, name, str);
                        else
                           _MESSAGE("Line %03d: </%s> <!-- trait end -->", line, name);
                        break;
                     case TileTemplateItem::kCode_TileStart:
                        _MESSAGE("Line %03d: <%s name=\"%s\"> <!-- tile start -->", line, RE::TagIDToName(token->result), str);
                        break;
                     case TileTemplateItem::kCode_TileEnd:
                        _MESSAGE("Line %03d: </%s> <!-- tile end -->", line, RE::TagIDToName(token->result));
                        break;
                     case TileTemplateItem::kCode_ConstTrait:
                        if (str)
                           _MESSAGE("Line %03d: <%s>%s</%s> <!-- reads as value %f -->", line, name, str, name, token->tagType);
                        else
                           _MESSAGE("Line %03d: <%s>%f</%s>", line, name, token->tagType, name);
                        break;
                     case TileTemplateItem::kCode_ConstOperator:
                        _MESSAGE("Line %03d: CONST OPERATOR | %f / %08X / %s", line, token->tagType, token->result, str);
                        break;
                     case TileTemplateItem::kCode_SrcOperator:
                        {
                           auto opName = RE::TagIDToName(token->result);
                           auto srcTrait = RE::TagIDToName((UInt32)token->tagType);
                           _MESSAGE("Line %03d: <%s src=\"%s\" trait=\"%s\" />", line, opName, str, srcTrait);
                        }
                        break;
                     case TileTemplateItem::kCode_TextContent:
                        _MESSAGE("Line %03d: TEXT CONTENT | %f / %08X / %s", line, token->tagType, token->result, str);
                        break;
                     case TileTemplateItem::kCode_AttributeName:
                        _MESSAGE("Line %03d: ATTRIBUTE: NAME  | %f / %08X / %s", line, token->tagType, token->result, str);
                        break;
                     case TileTemplateItem::kCode_AttributeSrc:
                        _MESSAGE("Line %03d: ATTRIBUTE: SRC   | %f / %08X / %s", line, token->tagType, token->result, str);
                        break;
                     case TileTemplateItem::kCode_AttributeTrait:
                        _MESSAGE("Line %03d: ATTRIBUTE: TRAIT | %f / %08X / %s", line, token->tagType, token->result, str);
                        break;
                     case TileTemplateItem::kCode_MQTextContent:
                        if (str)
                           _MESSAGE("Line %03d: %s <!-- MenuQue text content --> <!-- reads as value %f -->", line, str, token->tagType);
                        else
                           _MESSAGE("Line %03d: %f <!-- MenuQue text content, but no string is defined; this is a float -->", line, token->tagType);
                        break;
                     default:
                        _MESSAGE("Line %03d: Code %03X | %08X | %f / %s", line, token->unk00, token->result, token->tagType, str);
                  }
               }
               _MESSAGE("");
            }
            __declspec(naked) void Outer() {
               _asm {
                  push ecx; // protect
                  push eax;
                  call Inner; // _stdcall
                  pop  ecx; // restore
                  mov  eax, 0x00590330; // reproduce patched-over call to Tile::CreateTemplatedChildren
                  call eax;             //
                  mov  ecx, 0x00590480;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x0059047B, (UInt32)&Outer);
            }
         }
         void Apply() {
            LogFilename::Apply();
            LogStage1::Apply();
         }
      }
      //*/
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
         //TagParseDebug::Apply();
      }
   }
}