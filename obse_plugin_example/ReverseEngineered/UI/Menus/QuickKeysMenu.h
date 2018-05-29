#pragma once
#include "obse/GameExtraData.h"
#include "obse/GameMenus.h"

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
   };
   struct QuickKeysStaticData { // at 0x00B3B418; initialized by QuickKeysMenu constructor
      bool   unk00[8]; // 00 ... 07 // initialized to zero
      UInt8  unk08 = 0; // bool?
      UInt8  unk09;
      UInt8  unk0A;
      UInt8  unk0B;
      SInt32 unk0C = -1; // measurement in milliseconds
      SInt32 unk10 = -1; // measurement in milliseconds // same type as unk0C; could be "previous?"
      SInt32 unk14 = -1; // measurement in milliseconds // gets compared to GMST:iQuickKeyIgnoreMillis // same type as unk10; could be "previous previous?"
      SInt32 unk18 = -1; // quick key index, sign-extended; can be -1
      SInt32 unk1C = -1; // same type as unk18; could be "previous?"
      SInt32 unk20 = -1; // same type as unk1C; could be "previous previous?"
   };
   extern QuickKeysStaticData*     g_quickKeysStaticData;
   extern NiTPointerList<TESForm>* g_quickKeyList;

   //
   // Continue research from 005C1F70; it calls 005C1060 based on which quick key is pressed
   //
};