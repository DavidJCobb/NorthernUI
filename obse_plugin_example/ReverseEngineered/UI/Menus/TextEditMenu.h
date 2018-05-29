#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameMenus.h"

namespace RE {
   class TextEditMenu : public Menu { // sizeof == 0x5C
      public:
         TextEditMenu();
         ~TextEditMenu();

         enum {
            kTextEditValue_Prompt = kTileValue_user0,
         };

         // fields
         TileText*  text;  // 28
         TileImage* unk2C; // 2C
         TileImage* unk30; // 30
         MenuTextInputState unk34; // 34

         //
         // This class never supplies font or wrapwidth trait values to its MenuTextInputState 
         // instance. The MenuTextInputState struct handles text entry and limits the amount of 
         // entered text to what can fit on one line. Without a wrapwidth, MenuTextInputState 
         // will limit text to the (normalized) screen width; and without a font, it will measure 
         // text using font ID#2 (fontInfos[1]). Unless font #2 is a very large font, none of 
         // this should matter in any practical situation, but it's worth knowing.
         //
   };
   static_assert(sizeof(TextEditMenu) <= 0x5C, "RE::TextEditMenu is too large!");
   static_assert(sizeof(TextEditMenu) >= 0x5C, "RE::TextEditMenu is too small!");

   DEFINE_SUBROUTINE_EXTERN(void, ShowTextEditMenu, 0x005DCF20, const char* prompt, const char* defaultText);
   extern BSStringT* const g_textEditResult; // nullptr if the player canceled. Value may have padding spaces even if the player never typed any.
};