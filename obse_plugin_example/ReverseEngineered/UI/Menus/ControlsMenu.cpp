#include "ControlsMenu.h"

namespace RE {
   DEFINE_SUBROUTINE(void, CloseControlsMenu, 0x0059B9E0);

   SInt32 ControlsMenu::getMenuPage() {
      return CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(RE::kTagID_user0);
   }
   bool ControlsMenu::isInMappings() {
      return CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(RE::kTagID_user0) == 2.0F;
   }
   bool ControlsMenu::isRemapping() {
      return this->controlToRemap != nullptr;
   }
   void ControlsMenu::backOutOfMapping() {
      if (this->isRemapping())
         return;
      CALL_MEMBER_FN(this->tile, UpdateFloat)(RE::kTagID_user0, 1.0F);
   }
}