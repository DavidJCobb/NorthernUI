#pragma once
#include "ReverseEngineered/_BASE.h"
#include "obse/GameTypes.h"

class BSScissorTriShape;
struct NiColorAlpha;
class NiD3DShaderConstantMapEntry;
class NiPoint3;
class NiTexturingProperty;

//
// GOALS:
//
//  - Add a new tile trait, xxnFontPath, which overrides the font trait and allows us to 
//    specify the path of a FNT file, which our plug-in will load and use for text rendering.
//
//  - Avoid incompatibilites with MenuQue.
//
// CONSIDERATIONS:
//
//  - AlchemyMenu::UnkA0 stores font indices and wrapwidths.
//
//  - Several RaceSexMenu methods and related functions call FontManager::FontInfo methods.
//

namespace RE {

   //
   // FINDINGS:
   //
   //  - Font index 3 has a hardcoded extra line spacing of 6px, implemented at 005766E0, 
   //    00575B98, and possibly other places.
   //

   struct Struct00575610 { // only used in FontManager::FontInfo::RenderText
      Struct00575610(UInt32 a, UInt32 b, UInt32 c, UInt32 d, UInt32 e) : wrapwidth(a), wraplimit(b), wraplines(c), unk14(d), unk1C(e) {}

      struct Unk20 { // sizeof == 8
         //
         // Each of these appears to correspond to a character, or possibly just a 
         // printable character, in the struct's string. They appear to be listed 
         // in reverse order, i.e. struct->unk20.unk00 is the value for the last 
         // character in the string.
         //
         SInt32 unk00; // related to text X-coordinates: for right-aligned text, "current X" gets initialized to (0 - structInstance.GetUnk20Value(0));
         Unk20* next; // 04
      };

      BSStringT string; // 00 // string to render, most likely
      UInt32 wrapwidth; // 08 // 0x7FFFFFFF if not set
      UInt32 wraplimit; // 0C // 0x7FFFFFFF if not set
      UInt32 wraplines; // 10 // 0x7FFFFFFF if not set
      UInt32 unk14;     // 14 // 0x7FFFFFFF if not set
      UInt32 unk18 = 0; // index of last char (null terminator) in (string.m_data); gets set at 005763D7
      char   unk1C;     // 1C
      UInt8  pad1D[3];
      Unk20  unk20;     // 20
      //
      // ...?

      SInt32 GetUnk20Value(UInt32 index) { // 00573D20
         auto current = &this->unk20;
         if (index) {
            UInt32 i = 0;
            do {
               if (!current)
                  return -1;
               current = current->next;
            } while (++i < index);
         }
         if (!current)
            return -1;
         return current->unk00;
      };
   };

   struct FNTFile { // sizeof == 0x3928
      struct GlyphInfo { // sizeof == 0x38
         struct UV {
            union { // x or u: percentage of width
               float x;
               float u;
            };
            union { // y or v: percentage of height
               float y;
               float v;
            };
         };
         //
         float unk00;       // 00
         UV    topLeft;     // 04
         UV    topRight;    // 0C
         UV    bottomLeft;  // 14
         UV    bottomRight; // 1C
         float width;       // 24 // px
         float height;      // 28 // px
         float kerningLeft; // 2C // px // negative values pull adjacent letters closer
         float kerningRight;// 30 // px // negative values pull adjacent letters closer
         float ascent;      // 34 // px
      };
      //
      float     fontSize;
      SInt32    unk04;       // 0004 // Number of font textures. No more than 8 are allowed. See 00574892.
      UInt32    unk08;
      char      name[284];   // 000C // One texture filename (sans path) every 0x24 bytes, e.g. name[0], name[0x24], name[0x48], ...
      GlyphInfo glyphs[256]; // 0128
      //
      // Question: what would multiple texture files accomplish? How would they even work? It seems 
      // like the FontInfo struct only supports one texture file at a time in the first place.
      //
      // If you ensure that there is only one font texture, then you can have the name as long as 
      // you like. TEX names aren't capped to 0x24 bytes; the game just starts reading at multiples 
      // of 0x24 within the name string.
   };

   class FontManager {
      public:
         FontManager();
         ~FontManager();

         // 3C
         struct FontInfo {
            FontInfo();
            FontInfo(UInt32 id, const char* path) {
               ThisStdCall(0x005757A0, this, id, path, true);
            };
            ~FontInfo();

            UInt16   bLoadedSuccessfully;         // 00
            UInt16   pad02[1];
            char*    path;                        // 04 // "Data\\Fonts\\XXX.fnt"
            UInt32   ID;                          // 08 // 1-5 for default game fonts
            NiTexturingProperty* textureProperty; // 0C // During font load, this is created with a NiPixelData created from the TEX file.
            UInt32   unk10[(0x2C - 0x10) >> 2];	  // 10
            float    unk2C;                       // 2C // CONFIRMED: max of these values: (fnt->fontSize - glyph ascent) + glyph height
            float    unk30;                       // 30 // max of something (set during load), but I can't understand the setter code // largest kerningRight?
            NiD3DShaderConstantMapEntry* unk34;   // 34
            FNTFile* fnt;                         // 38

            static FontInfo* Load(const char* path, UInt32 ID);
            bool GetName(char* out);	// filename stripped of path and extension

            MEMBER_FN_PREFIX(FontInfo);
            
            // Creates a BSScissorTriShape* with the requisite number of vertices for rendering a 
            // string of text, with each character set up as a quad. Vertices for each quad are in the 
            // following order: upper-left, lower-left, upper-right, lower-right.
            //
            DEFINE_MEMBER_FN(MakeTriShape, BSScissorTriShape*, 0x00574200, UInt32 chars, NiColorAlpha* defaultColor);

            // Updates the (quadIndex)-th character in a BSScissorTriShape* created with MakeTriShape. 
            // The (state) parameter should represent where you are currently drawing (i.e. the upper-
            // left corner of the (quadIndex)-th character), and it will be modified by this subroutine 
            // to account for the width and kerning of the character.
            //
            // Given that 2D Y == 3D -Z, the upper and lower edges of the glyph are as follows:
            //
            // Upper Z = Current Z + Ascent          | Upper Y = Current Y - Ascent
            // Lower Z = Current Z + Ascent - Height | Lower Y = Current Y - Ascent + Height
            //
            DEFINE_MEMBER_FN(UpdateTriShapeQuad, void, 0x00573F10, FNTFile::GlyphInfo*, UInt32 quadIndex, BSScissorTriShape* shape, NiPoint3* state, NiColorAlpha* letterColor);

            // Creates a BSScissorTriShape and renders text to it using the above two methods. All text 
            // options available to Oblivion XML can be specified here, with the exception of HTML 
            // options.
            //
            // Unused wrap arguments should be set to max signed int, i.e. 0x7FFFFFFF, as 
            // is done in Bethesda's code. Wrap arguments may be modified by this function.
            //
            // lineBreak should be '\n' and always is in Bethesda's code.
            //
            // colorData can be null.
            //
            // useVerticalAlign should be true and always is in Bethesda's code. Vertical alignment in 
            // this case refers to a Z-offset applied to the first line, which computes to:
            //
            //    -((FontInfo::unk2C - FNT::fontSize) * 2 + HardcodedLineSpacing)
            //
            // where HardcodedLineSpacing equals 6 for the font at index 3, and 0 for all other fonts. 
            // Of course, FontInfo::unk2C itself is just the largest (fontSize + height - ascent) of 
            // all glyphs, and so this computes to (height - ascent) * 2 + HardcodedLineSpacing given 
            // the glyph with the largest difference between its height and ascent.
            //
            DEFINE_MEMBER_FN(RenderText, BSScissorTriShape*, 0x00576670, BSStringT* string, SInt32* wrapwidth, SInt32* wraplimit, UInt32 unk4, SInt32 wraplines, UInt32 justify, char lineBreak, NiColorAlpha* colorData, bool useVerticalAlign);
         };

         FontInfo* fontInfos[5]; // 00 indexed by FontInfo::ID - 1; access inlined at each point in code where font requested
         UInt8     unk14;        // 14
         UInt8     pad15[3];

         static FontManager* GetSingleton();
   };

   static_assert(sizeof(RE::FNTFile::GlyphInfo) <= 0x38, "RE::FNTFile::GlyphInfo is too large!");
   static_assert(sizeof(RE::FNTFile::GlyphInfo) >= 0x38, "RE::FNTFile::GlyphInfo is too small!");
   static_assert(sizeof(RE::FNTFile) <= 0x3928, "RE::FNTFile is too large!");
   static_assert(sizeof(RE::FNTFile) >= 0x3928, "RE::FNTFile is too small!");
   static_assert(sizeof(FontManager) <= 0x18, "RE::FontManager is too large!");
   static_assert(sizeof(FontManager) >= 0x18, "RE::FontManager is too small!");
   static_assert(sizeof(FontManager::FontInfo) <= 0x3C, "RE::FontManager::FontInfo is too large!");
   static_assert(sizeof(FontManager::FontInfo) >= 0x3C, "RE::FontManager::FontInfo is too small!");
};