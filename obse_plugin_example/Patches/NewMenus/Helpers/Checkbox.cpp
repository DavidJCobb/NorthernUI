#include "Checkbox.h"

namespace XXNMenuHelpers {
   bool Checkbox::HandleClick(RE::Tile* tile) {
      if (!tile || tile != this->tile)
         return false;
      this->state = !this->state;
      this->Render();
      return true;
   };
   void Checkbox::Render() const {
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_CheckboxState, this->state ? 2.0F : 1.0F);
   };
   void Checkbox::Set(bool v) {
      this->state = v;
      this->Render();
   };
};