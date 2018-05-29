#include "MenuTextInputState.h"
#include "obse/GameAPI.h"

namespace RE {
   MenuTextInputState* MenuTextInputState::Create() {
      auto result = (MenuTextInputState*) FormHeap_Allocate(sizeof(MenuTextInputState));
      if (result)
         CALL_MEMBER_FN(result, Constructor)();
      return result;
   };
};