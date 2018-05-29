#include "Logging.h"
#include "obse_common/SafeWrite.h"

#include "Services/INISettings.h"

namespace CobbPatches {
   namespace Logging {
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
      }
   }
}