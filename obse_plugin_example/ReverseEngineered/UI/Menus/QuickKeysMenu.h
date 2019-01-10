#pragma once
#include "obse/GameExtraData.h"
#include "obse/GameMenus.h"
#include "shared.h"

namespace RE {
   class QuickKeysMenu : public Menu {
      public:
         enum { kID = 0x416 };
         //
         TileText*  shortcutText; // 28
         TileRect*  selector;     // 2C
         TileImage* shortcutIcon[8]; // 30 ... 4C
         //
         enum {
            kTileID_MinToUnbindShortcut = 0x33,
         };
         //
         MEMBER_FN_PREFIX(QuickKeysMenu);
         DEFINE_MEMBER_FN(Subroutine005C25C0, void, 0x005C25C0, TESForm*); // assigns a form to the currently highlighted quick key?
         DEFINE_MEMBER_FN(Subroutine005C1A70, void, 0x005C1A70, UInt32 formType, bool bookOptions); // removes forms of the specified type from the currently highlighted quick key, with extra options for books/scrolls
   };
   struct QuickKeysStaticData { // at 0x00B3B418; initialized by QuickKeysMenu constructor
      bool   unk00[8]; // 00 ... 07 // initialized to zero
      UInt8  unk08 = 0; // most likely a bool
      UInt8  unk09;
      UInt8  unk0A;
      UInt8  unk0B;
      SInt32 unk0C = -1; // measurement in milliseconds
      SInt32 unk10 = -1; // measurement in milliseconds // same type as unk0C; could be "previous?"
      SInt32 unk14 = -1; // measurement in milliseconds // gets compared to GMST:iQuickKeyIgnoreMillis // same type as unk10; could be "previous previous?"
      SInt32 currentKey = -1; // 18 // the quick key index currently being pressed, or -1 if none is being pressed; updated every frame
      SInt32 unk1C = -1; // same type as currentKey; could be "previous?"          if the current key is released, this is also set to -1
      SInt32 unk20 = -1; // same type as unk1C;      could be "previous previous?" if the current key is released, this is also set to -1
      //
      // I found a function that iterates over the fields as if they're arrays -- specifically, 
      // SInt32 unk0C[3] and SInt32 unk18[3].
   };
   extern QuickKeysStaticData*     g_quickKeysStaticData;
   extern NiTPointerList<TESForm>* g_quickKeyLists; // this is an array of eight items, i.e. &g_quickKeyList[1] == (char*)&g_quickKeyList[0] + sizeof(NiTPointerList)

   extern bool* g_quickKeySelectionShowing;

   DEFINE_SUBROUTINE_EXTERN(void, HideQuickKeyMenuSelection, 0x005C1000);
   DEFINE_SUBROUTINE_EXTERN(void, ShowQuickKeyMenuSelection, 0x005C1B80, SInt32 which);
   //
   // Do not call these directly:
   //
   DEFINE_SUBROUTINE_EXTERN(void, QuickKeyFrameHandler, 0x005C1F70);
   DEFINE_SUBROUTINE_EXTERN(void, OnQuickKeyPressed,    0x005C1060, SInt32 which); // always called by the frame handler; if no quick key is down, then which == -1

   //
   // 00422BA0(UInt32)           == BaseExtraList::SetExtraQuickKey; only used for items (spells have no extra-data)
   // 004895B0(TESForm*, SInt32) == ExtraContainerChanges::Data member function; removes ExtraQuickKey data from the item
   //
};