#include "_BASE.h"
#include "obse/Utilities.h"

namespace RE {
   BSStringT::~BSStringT() {
      ThisStdCall(0x00402DA0, this); // NOTE: If this gets inlined everywhere it's used, then BSStringT::~BSStringT itself may still compile as an empty subroutine
   };
   bool BSStringT::IsEqual(BSStringT* other, bool caseSensitive) const {
      std::string a;
      std::string b;
      this->ToStdString(&a);
      other->ToStdString(&b);
      if (!caseSensitive) {
         std::transform(a.begin(), a.end(), a.begin(), tolower);
         std::transform(b.begin(), b.end(), b.begin(), tolower);
      }
      return a.compare(b) == 0;
   };
   void BSStringT::ToStdString(std::string* out) const {
      if (this->m_dataLen != 0xFFFF)
         out->assign(this->m_data, this->m_dataLen);
      else
         out->assign(this->m_data);
   };

   DEFINE_SUBROUTINE(void, SprintfBSStringT, 0x00402E30, BSStringT* out, const char* format, ...);
};