#include "QuickKeysMenu.h"

namespace RE {
   QuickKeysStaticData*     g_quickKeysStaticData = (QuickKeysStaticData*)     0x00B3B418;
   NiTPointerList<TESForm>* g_quickKeyLists       = (NiTPointerList<TESForm>*) 0x00B3B440;

   bool* g_quickKeySelectionShowing = (bool*) 0x00B3B43D;

   DEFINE_SUBROUTINE(void, HideQuickKeyMenuSelection, 0x005C1000);
   DEFINE_SUBROUTINE(void, ShowQuickKeyMenuSelection, 0x005C1B80, SInt32 which);

   DEFINE_SUBROUTINE(void, QuickKeyFrameHandler, 0x005C1F70);
   DEFINE_SUBROUTINE(void, OnQuickKeyPressed,    0x005C1060, SInt32 which);
};