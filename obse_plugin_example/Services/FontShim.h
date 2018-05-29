#pragma once
#include <mutex>
#include <unordered_map>
#include <vector>

#include "ReverseEngineered/UI/FontManager.h"

//
// This class stores fonts specified by path (using the xxnFontPath trait instead of the 
// FONT trait), and is responsible for accessing these fonts (in conjunction with a number of 
// patches to the game's code).
//
// Vanilla font indices are [1, 5] (or [0, 4] internally).
//
// Fonts-by-path have an ID value that is equal to their index within FontShim::_fontsByPath, 
// shifted left by one byte and OR'd with 0xFF. This means that the zeroth font-by-path has 
// an ID of 255; the first, 511; the second, 767, and so on. FontShim::ToVectorIndex can take 
// these IDs and convert them back to indices within the vector.
//
// We patch every site where the game reads a tile's FONT trait: if the tile specifies the 
// xxnFontPath trait, then we ignore FONT and use xxnFontPath. We retrieve the font with the 
// given path (loading it on demand) and return its ID (the vector index encoded as described 
// above). We also patch every site where the game actually uses IDs pulled from the FONT 
// trait to retrieve a FontInfo*, and have them call FontShim::GetFont instead. That member 
// function checks whether an ID is greater than 31 (the max font index with MenuQue active; 
// max index without MenuQue is 5), and if so, it calls ToVectorIndex function to get the 
// index within _fontsByPath before then plucking out the proper FontInfo* and returning it.
//
// It's all very indirect, but it allows us to maintain the existing code's structure. The 
// only other option would be to NOP over large chunks of Bethesda's code and rewrite it all.
//

class FontShim { // singleton
   typedef RE::FontManager::FontInfo FontInfo;

   private:
      static bool   IsVanillaIndex(UInt32 index);
      static UInt32 ToVectorIndex (UInt32 index); // takes an index and shifts it one byte to the right, to allow use as a vector index
      //
      struct _Entry {
         std::string path;
         FontInfo*   font;
         //
         _Entry(const char* path, FontInfo* font) : path(path), font(font) {};
      };
      std::vector<_Entry> _fontsByPath;
      std::recursive_mutex _writeLock;
      //
   public:
      static FontShim& GetInstance();
      static bool      Patch();
      //
      FontInfo* GetFont(UInt32 index); // vanilla indexes should start from 0
      FontInfo* GetFont(const char* path);
      UInt32    GetIndex(const char* path); // returns a vanilla index (zero-indexed) or a vector index shifted to the left by one byte
      //
      static FontInfo* GetFontASMCall(UInt32 index) {
         return FontShim::GetInstance().GetFont(index);
      }
      //
      ~FontShim();
};