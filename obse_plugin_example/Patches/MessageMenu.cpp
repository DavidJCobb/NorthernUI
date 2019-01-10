#include "Patches/MessageMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/MessageMenu.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace MessageMenu {
      namespace FixOBSETextEntry {
         //
         // OBSE's OpenTextInput command lets you use the TextEditMenu, MessageMenu, or 
         // BookMenu as textboxes. However, their code for MessageMenu is broken: they 
         // try to write the prompt and inputted value into the message text, but they 
         // do it like this:
         //
         //    msgBox->messageText->SetStringValue(kTileValue_string, text.c_str());
         //    msgBox->messageText->UpdateField(kTileValue_string, 0.0, text.c_str());
         //
         // Essentially, they're cheating a little bit. If you call Tile::UpdateString, 
         // it sets the string value, processes all incoming operators, and then calls 
         // UpdateField. OBSE is skipping that middle step... which means that if any 
         // tile in the menu does this:
         //
         //    <string> <copy src="theMessageTextTile" trait="string" /> </string>
         //
         // That "dependent" tile won't be updated.
         //
         // NorthernUI breaks under this scenario. In order to be compatible with 
         // Enhanced Economy, we had to make the message-text tile invisible, and have 
         // a separate tile copy its string and display it. That separate tile isn't 
         // updated by OBSE, which breaks any OBSE-powered text inputs that are embedded 
         // into MessageMenu.
         //
         // We fix this by patching a frame handler into MessageMenu, to check for any 
         // changes and properly commit them if they're detected. Unfortunately, there 
         // isn't much we can do to optimize this:
         //
         //  - Blindly committing changes every frame seems like it'd be less efficient 
         //    than doing a string comparison.
         //
         //  - We can't just check for string length as a heuristic, because you can 
         //    move a cursor (rendered as a glyph within the string) within text, which 
         //    will change the rendered string without affecting its length.
         //
         static std::string s_lastString;
         //
         void _stdcall Inner(RE::MessageMenu* menu) {
            auto tile = menu->tileMessageText;
            if (!tile)
               return;
            auto trait = CALL_MEMBER_FN(tile, GetOrCreateTrait)(RE::kTagID_string);
            if (!trait)
               return;
            auto& str = trait->str;
            //
            bool changed = (!str.m_data && !s_lastString.empty());
            if (!changed)
               changed = (s_lastString.compare(0, str.m_dataLen, str.m_data) != 0);
            //
            if (changed) {
               s_lastString.assign(str.m_data, str.m_dataLen);
               //
               // UpdateString only calls DoActionEnumeration (the step that OBSE skips) 
               // if the string is changed. Let's clear the trait value before we set it 
               // to itself, just to make sure the game thinks it's changing.
               //
               CALL_MEMBER_FN(&str, Replace_MinBufLen)("", 0);
               CALL_MEMBER_FN(tile, UpdateString)(RE::kTagID_string, s_lastString.c_str());
            }
         }
         __declspec(naked) void Outer() {
            _asm {
               push ecx;
               call Inner;
               retn;
            }
         }
         void Apply() {
            SafeWrite32(RE::MessageMenu::kVTBL + (4 * 0xB), (UInt32)&Outer);
         }
      }
      //
      void Apply() {
         FixOBSETextEntry::Apply();
      }
   };
};