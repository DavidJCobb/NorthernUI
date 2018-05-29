#pragma once
#include "obse/GameTypes.h"

namespace RE {
   struct MenuTextInputState { // constructor: 0x0057FE70 // sizeof == 0x28
      //
      // This struct is used to keep track of the state of a text input within a menu. 
      // The TextEditMenu includes this struct as a field, while other menus with text 
      // fields (e.g. AlchemyMenu for the potion name, or RaceSexMenu for the character 
      // name) create instances of this struct on the heap and store a pointer to them.

      bool   hasFocus;            // 00 // setting this via 0057DD90 also updates cursor pos // "has focus" / "is ready" bool? menus ignore keypresses if this is false
      UInt8  pad01[3];
      UInt32 cursorPosition;      // 04
      bool   cursorBlinkState;    // 08 // if true, we show '|'; otherwise we show font char 0x7F
      UInt8  pad09[3];
      SInt32 textWrapwidth = -1;  // 0C // text wrap width minus 5; -1 means unspecified (and is replaced with a normalized screen width)
      UInt32 lastCursorBlinkTime; // 10 // number of milliseconds since cursorBlinkState was last toggled
      UInt32 fontIndex = 1;       // 14 // text font index; init'd to 1; zero-indexed
      BSStringT editValue;        // 18 // value being edited
      BSStringT renderedString;   // 20 // rendered string, i.e. with a '|' glyph inserted at the cursor position and possibly '--' where words break when wrapping

      //
      // When a keypress is received from the menu (via HandleKeypress), we commit the 
      // change to our string if-and-only-if the text would not word wrap (per the 
      // TextFitsOnLine method). That's why we store our font index: so we can grab the 
      // font from the font manager. (Of course, we skip the word-wrap check if we are 
      // deleting a character.)
      //
      // HandleKeypress also handles our cursorPosition in response to special key codes. 
      // The special key codes it responds to appear to be:
      //
      //  0 | Backspace   | Deletes a character on one side of the cursor, if it's not at the start of the string.
      //  1 | Arrow Left  | Moves cursor leftward if it is not at the start of the string.
      //  2 | Arrow Right | Moves cursor rightward if it is not at the end of the string.
      //  3 | Arrow Up    | No special handling. Treated as any other character (which in this case writes garbage charcode 03 to the edit value).
      //  4 | Arrow Down  | No special handling. Treated as any other character (which in this case writes garbage charcode 04 to the edit value).
      //  5 | Home        | Moves cursor to the start of the string.
      //  6 | End         | Moves cursor to the right end of the string.
      //  7 | Del         | Deletes a character on one side of the cursor, if it's not at the end of the string.
      //  8 | Enter       | Sets hasFocus to false.
      //  9 | PgUp OR Alt | Returns immediately.
      // 10 | PgDown      | Returns immediately.
      //

      static MenuTextInputState* Create();

      MEMBER_FN_PREFIX(MenuTextInputState);
      //
      DEFINE_MEMBER_FN(Constructor, MenuTextInputState*, 0x0057FE70);
      DEFINE_MEMBER_FN(Destructor,  void,                0x0057FEB0);
      //
      DEFINE_MEMBER_FN(GetEditValue,     const char*, 0x00438DB0);
      DEFINE_MEMBER_FN(HandleKeypress,   void,   0x0057FF50, unsigned char code);  // (code) is a keypress code passed directly from Menu::HandleKeypress.
      DEFINE_MEMBER_FN(SetHasFocus,      void,   0x0057DD90, bool   value); // when set to true, cursor is moved to the end of the edit value
      DEFINE_MEMBER_FN(SetFontID,        void,   0x0057D2D4, UInt32 index); // if the index (which should be a raw font trait) is not 1, 2, or 3, then this method stores 0 instead
      DEFINE_MEMBER_FN(SetStringsTo,     void,   0x0057FF20, const char* text); // sets rendered string and actual value
      DEFINE_MEMBER_FN(SetTextWrapwidth, SInt32, 0x00583DD0, SInt32 width);
      DEFINE_MEMBER_FN(TextFitsOnLine,   bool,   0x0057DE00, char*  text);  // this is why we track our font index
      //
      DEFINE_MEMBER_FN(UpdateCursorBlinkState, void, 0x0057DDE0); // your menu should call this every frame if this object has focus, and then update menu-related text input state after the call
      //
      DEFINE_MEMBER_FN(UpdateRenderedStringForCursorMovement, const char*, 0x00580120); // updates the renderedString to be equivalent to the editValue with "|" inserted at the cursor position; returns the string
   };
   static_assert(sizeof(MenuTextInputState) <= 0x28, "RE::MenuTextInputState is too large!");
   static_assert(sizeof(MenuTextInputState) >= 0x28, "RE::MenuTextInputState is too small!");
};