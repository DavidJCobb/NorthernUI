#include "FontManager.h"

namespace RE {
   FontManager* FontManager::GetSingleton() {
      auto subroutine = (FontManager*(*)())0x00576A30; // (type(*)(args...))address
      return subroutine();
   }

   FontManager::FontInfo::~FontInfo() {
      ThisStdCall(0x00573E70, this);
   };
};