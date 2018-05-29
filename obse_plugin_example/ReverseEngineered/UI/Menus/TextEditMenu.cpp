#include "TextEditMenu.h"

namespace RE {
   DEFINE_SUBROUTINE(void, ShowTextEditMenu, 0x005DCF20, const char* prompt, const char* defaultText);
   extern BSStringT* const g_textEditResult = (BSStringT*) 0x00B3B738;
};