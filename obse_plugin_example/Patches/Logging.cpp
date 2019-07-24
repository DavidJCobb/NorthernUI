#include "Logging.h"
#include "obse_common/SafeWrite.h"

#include "ReverseEngineered/UI/Tile.h"
#include "Services/INISettings.h"
#include "Services/TileDump.h"

namespace CobbPatches {
   namespace Logging {
      namespace UICyclicalTraitReference {
         void Log(const char* message) {
            _MESSAGE(message);
         }
         void _stdcall Inner(RE::Tile::Value* target) {
            _MESSAGE(" [[Full Trace]]");
            auto state = RE::TileParseState::GetInstance();
            for (UInt32 i = 0; i < state->valuesCount; i++) {
               auto value = state->values[i];
               if (!value)
                  break;
               auto mark  = " ";
               if (value == target)
                  mark = ">";
               auto trait = RE::TagIDToName(value->id);
               if (!trait)
                  trait = "<NO TRAIT>";
               auto name  = "<NO TILE>";
               auto tile  = value->owner;
               if (tile) {
                  name = tile->name.m_data;
                  if (!name || name[0] == '\0')
                     name = "<UNNAMED TILE>";
               }
               _MESSAGE("    %s %s::%s", mark, name, trait);
            }
            _MESSAGE(" [[Trace Complete]]");
            //TileCyclicalReferenceChecker::GetInstance().Check(target);
            TileCyclicalReferenceChecker::GetInstance().Check(state->values[0]);
         }
         __declspec(naked) void Outer() {
            _asm {
               push edi;
               call Inner; // stdcall
               mov  eax, dword ptr [edi];       // reproduce patched-over instruction
               mov  eax, dword ptr [eax + 0x8]; //
               mov  ecx, 0x0058BBBE;
               jmp  ecx;
            }
         }
         void Apply() {
            WriteRelCall(0x0058BB5E, (UInt32)&Log);
            WriteRelCall(0x0058BB68, (UInt32)&Log);
            WriteRelCall(0x0058BB72, (UInt32)&Log);
            WriteRelCall(0x0058BBA6, (UInt32)&Log);
            WriteRelCall(0x0058BBD3, (UInt32)&Log);
            WriteRelCall(0x0058BBDD, (UInt32)&Log);
            //
            WriteRelJump(0x0058BBB9, (UInt32)&Outer);
         }
      }

      void Log004A7A60(const char* format,...) {
         va_list args;
         va_start(args, format);
         std::string formatPrefix("Log received from endpoint 0x004A7A60: ");
         formatPrefix += format;
         {  // Some of Bethesda's format messages are wrong.
            std::size_t found = formatPrefix.find("%S", 20);
            while (found != std::string::npos) {
               formatPrefix[found + 1] = 's';
               found = formatPrefix.find("%S", found);
            }
         }
         gLog.Log(IDebugLog::kLevel_Message, formatPrefix.c_str(), args);
         va_end(args);
      };
      void Log00585F40(UInt32 maybeScriptStateObj, const char* format, ...) {
         va_list args;
         va_start(args, format);
         std::string formatPrefix("Log received from endpoint 0x00585F40: ");
         formatPrefix += format;
         gLog.Log(IDebugLog::kLevel_Message, formatPrefix.c_str(), args);
         va_end(args);
      };

      void __stdcall LogSingle(const char* str) {
         _MESSAGE("Log received from endpoint <KERNEL32.OutputDebugStringA>: %s", str);
      };
      void ReplaceOutputDebugStringACall(UInt32 address) {
         SafeWrite8(address, 0x90);
         WriteRelCall(address + 1, (UInt32)&LogSingle);
      }

      void Apply() {
         if (!NorthernUI::INI::Debug::bSwipeUnusedVanillaLogs.bCurrent)
            return;
         WriteRelJump(0x004A7A60, (UInt32)&Log004A7A60);
         WriteRelJump(0x00585F40, (UInt32)&Log00585F40);
         //
         // Replace all calls to KERNEL32.OutputDebugStringA:
         //
         ReplaceOutputDebugStringACall(0x00431DE4);
         ReplaceOutputDebugStringACall(0x004464BC);
         ReplaceOutputDebugStringACall(0x00568B7D);
         ReplaceOutputDebugStringACall(0x00584FC1);
         ReplaceOutputDebugStringACall(0x0077BFBE);
         ReplaceOutputDebugStringACall(0x008BB90B);
         ReplaceOutputDebugStringACall(0x008BB952);
         //
         UICyclicalTraitReference::Apply();
      }
   }
}