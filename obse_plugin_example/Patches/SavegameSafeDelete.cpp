#include "SavegameSafeDelete.h"
#include "obse/Serialization.h"

#include "obse_common/SafeWrite.h"

#include <SHELLAPI.H> // for deleting save files to the recycle bin

namespace CobbPatches {
   namespace SavegameSafeDelete {
      namespace DeleteToRecycleBin {
         //
         // Force Oblivion to delete save files to the Recycle Bin rather than just 
         // destroying them forever.
         //
         // This patch conflicts with an OBSE hook (because the OBSE hook replaces 
         // and reproduces Oblivion's own DeleteFileA call), so we have to reproduce 
         // the contents of that hook, as well as any functions that the hook used 
         // that weren't available in OBSE headers.
         //
         // For some reason, OBSE seems to apply or re-apply its hook very late. 
         // There's no OBSE-provided event we can use to patch after that moment but 
         // before the LoadgameMenu might open, so instead, we patch the menu's own 
         // constructor. Every time the menu is open, we redundantly apply the real 
         // patch (to delete to the Recycle Bin) overtop of OBSE's hook site.
         //
         // NOTE: This hook actually also applies to the SaveMenu, so we patch it 
         // in a similar manner. Could we just patch MainMenu instead?
         //
         static bool s_nextIsOverwrite = false; // overwriting a save file involves deleting it and then writing to that filename
         //
         namespace CopiedOBSE { // Anything copied from CPP files.
            std::string ConvertSaveFileName(std::string name) {
               // name not passed by const reference so we can modify it temporarily
               std::string	result;
               //
               // save off all of the ".bak" suffixes:
               //
               std::string	bakSuffix;
               int firstDotBak = name.find(".bak");
               if (firstDotBak != -1)  {
                  bakSuffix = name.substr(firstDotBak);
                  name = name.substr(0, firstDotBak);
               }
               //
               // change extension to .obse:
               //
               int lastPeriod = name.rfind('.');
               if (lastPeriod == -1)
                  result = name;
               else
                  result = name.substr(0, lastPeriod);
               result += ".obse";
               //
               // readd the ".bak" suffixes:
               //
               result += bakSuffix;
               //
               return result;
            }
         };

         void Recycle(const char* path) {
            if (strlen(path) >= MAX_PATH) {
               _MESSAGE("Unable to recycle file; the path was too long. <%s>", path);
               DeleteFile(path);
               return;
            }
            SHFILEOPSTRUCT operation;
            memset(&operation, 0, sizeof(operation));
            //
            char operationPath[MAX_PATH];
            memset(operationPath, 0, MAX_PATH);
            strcpy(operationPath, path);
            operationPath[MAX_PATH - 1] = '\0';
            //
            operation.hwnd   = 0;
            operation.wFunc  = FO_DELETE;
            operation.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_ALLOWUNDO;
            operation.pFrom  = operationPath;
            //
            SHFileOperation(&operation);
            _MESSAGE("Attempted to recycle file. <%s>", path);
         };
         void __stdcall Inner(const char* path) {
            //::Serialization::HandleDeleteGame(path); // SKSE code; see Hooks_SaveLoad.cpp
            {  // based on OBSE Serialization::HandleDeleteGame(path):
               std::string	savePath = CopiedOBSE::ConvertSaveFileName(path);
               if (s_nextIsOverwrite)
                  DeleteFile(savePath.c_str());
               else
                  Recycle(savePath.c_str());
            }
            if (s_nextIsOverwrite)
               DeleteFile(path);
            else
               Recycle(path);
            s_nextIsOverwrite = false;
         };

         namespace ButDontRecycleWhenOverwriting {
            __declspec(naked) void Inner() {
               _asm {
                  pushad;
                  mov  s_nextIsOverwrite, 0x01;
                  popad;
                  mov  eax, 0x00453480; // TESSaveLoadGame::DeleteSaveFile
                  call eax;
                  mov  eax, 0x0045F662;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x0045F65D, (UInt32)&Inner);
            }
         }

         void DelayedApply() {
            WriteRelCall(0x004534A6, (UInt32)&Inner); // TESSaveLoadGame::DeleteSaveFile+0x26
            _MESSAGE("Intercepted the MainMenu constructor. Patched the menu to delete save files to the Recycle Bin.");
            //
            ButDontRecycleWhenOverwriting::Apply();
         };

         /*__declspec(naked) void SetupHook_MainMenu() {
            _asm {
               mov  eax, 0x00584640;
               call eax;       // patched-over call to Menu::Menu()
               call DelayedApply;
               mov  eax, 0x005B5F4F;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005B5F4A, (UInt32)&SetupHook_MainMenu);
         };*/
      };

      void Apply() {
         //DeleteToRecycleBin::Apply();
      };
   };
};