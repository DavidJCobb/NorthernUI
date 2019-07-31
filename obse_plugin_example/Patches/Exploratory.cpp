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
      }
   }
}