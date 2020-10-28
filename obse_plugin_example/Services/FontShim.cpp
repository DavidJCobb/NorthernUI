#include "Services/FontShim.h"
#include "Miscellaneous/CRC24.h"
#include "obse_common/SafeWrite.h"

const UInt32 _maxFontIndex = 31; // MenuQue adds support for fonts 7 - 31.

bool FontShim::IsVanillaIndex(UInt32 index) {
   return index <= _maxFontIndex;
}
UInt32 FontShim::ToVectorIndex(UInt32 index) {
   return index >> 0x8; // lowest byte is just a sentinel to ensure that fonts-by-index and fonts-by-path do not conflict
}

FontShim& FontShim::GetInstance() {
   static FontShim fontShim;
   return fontShim;
}

FontShim::FontInfo* FontShim::GetFont(UInt32 index) {
   if (index == 0xFFFFFFFF) // XML files that incorrectly request font ID 0 will crash NorthernUI or access random memory in vanilla; return font 1 instead. example: DarNified Daedric main menu
      return RE::FontManager::GetSingleton()->fontInfos[0];
   if (FontShim::IsVanillaIndex(index))
      return RE::FontManager::GetSingleton()->fontInfos[index];
   index = FontShim::ToVectorIndex(index);
   if (index < this->_fontsByPath.size())
      return this->_fontsByPath[index].font;
   return nullptr;
}
FontShim::FontInfo* FontShim::GetFont(const char* path) {
   //
   //  - hash the font path
   //  - check if we have a FontInfo mapped to that hash
   //     - if not, load the font and then map it to that hash
   //  - return the FontInfo mapped to that hash
   //
   if (path[0] == ' ' && path[1] == '\0') { // Oblivion doesn't let you use "" for a blank trait; it forces to " "
      return nullptr;
   }
   auto& list = this->_fontsByPath;
   for (auto id = list.begin(); id != list.end(); id++) {
      if (id->path.compare(path) == 0) {
         //_MESSAGE("=== GetFont: Existing font pointer is %08X.", (UInt32) id->font);
         return id->font;
      }
   }
   this->_writeLock.lock();
   //
   // TODO: Double-check that the file exists. If it does not, then unlock, and return this->GetFont(0);
   //
   //_MESSAGE("=== GetFont: We are about to load a new font.");
   {  // Clean up after MenuQue.
      SafeWrite32(0x005748D2, (UInt32) "Data\\Fonts");
   }
   auto out = new FontInfo(0, path);
   if (out->bLoadedSuccessfully == 0) {
      //_MESSAGE("=== GetFont: Failed to load font. Bad font path was: %s", path);
      delete out;
      this->_writeLock.unlock();
      return nullptr;
   }
   list.emplace_back(path, out);
   UInt32 index = ((list.size() - 1) << 0x8) | 0xFF;
   out->ID = index;
   this->_writeLock.unlock();
   //_MESSAGE("=== GetFont: Loaded new font; index and path are: %08X / %s", index, path);
   //_MESSAGE("============ Font loaded successfully? %d.", out->bLoadedSuccessfully);
   //_MESSAGE("============ Font FNT line-height: %f", out->fnt->fontSize);
   return out;
}
UInt32 FontShim::GetIndex(const char* path) {
   auto font = this->GetFont(path);
   if (font)
      return font->ID;
   return 0;
}

FontShim::~FontShim() {
   this->_writeLock.lock();
   auto& list = this->_fontsByPath;
   for (auto i = list.begin(); i != list.end(); i++)
      delete i->font;
   list.clear();
   this->_writeLock.unlock();
}