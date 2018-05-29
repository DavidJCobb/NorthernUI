#include "Miscellaneous.h"

namespace RE {
   DEFINE_SUBROUTINE(bool,   FormIsGoldAsInCurrency, 0x00469980, TESForm* form);
   DEFINE_SUBROUTINE(UInt32, GetFormQuality,         0x0046E3F0, TESForm* form);
   DEFINE_SUBROUTINE(float,  GetFormWeight,          0x00470620, TESForm* form);
   DEFINE_SUBROUTINE(TESBipedModelForm*, GetTESBipedModelForm, 0x004691B0, TESForm* form);
   DEFINE_SUBROUTINE(float,  RoundToMultipleOf,      0x004843B0, float value, float divisor);
};