#include "InterfaceManager.h"
#include "ReverseEngineered/NetImmerse/NiObject.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/NiObjects.h"

namespace RE {
   SInt32* const SInt32ScreenWidth          = (SInt32*) 0x00B06C4C;
   SInt32* const SInt32ScreenHeight         = (SInt32*) 0x00B06C50;
   bool*   const bPlayerLevelUpIsQueued     = (bool*)   0x00B3A6D1;
   double* const fNormalizedScreenWidthDbl  = (double*) 0x00A688A0;
   double* const fNormalizedScreenHeightDbl = (double*) 0x00A68D70;
   float*  const fNormalizedScreenWidthF    = (float*)  0x00A688A8;
   float*  const fNormalizedScreenHeightF   = (float*)  0x00A68D78;
   InterfaceManager** const ptrInterfaceManager = (InterfaceManager**) 0x00B3A6E0;

   DEFINE_SUBROUTINE(float,  GetNormalizedScreenWidth,  0x0057D7A0);
   DEFINE_SUBROUTINE(float,  GetNormalizedScreenHeight, 0x0057D7F0);
   DEFINE_SUBROUTINE(float,  GetNormalizedScreenMinX,   0x0057E2D0); // Given (0, 0, 0) as the screen center.
   DEFINE_SUBROUTINE(float,  GetNormalizedScreenMaxX,   0x0057E330); //
   DEFINE_SUBROUTINE(float,  GetNormalizedScreenMinY,   0x0057E390); //
   DEFINE_SUBROUTINE(float,  GetNormalizedScreenMaxY,   0x0057E3F0); //
   
   DEFINE_SUBROUTINE(float,  GetNewMenuDepth,                   0x00584980); // returns highest menu depth + 2 (suitable for use on a new menu); also updates cursor to be 100 depth-values in front of highest menu
   DEFINE_SUBROUTINE(UInt32, GetTopmostMenuIDUnderUnkCondition, 0x00578FE0);
   DEFINE_SUBROUTINE(void,   PlayUIClicksound,                  0x0057DE50, UInt32 index);
   DEFINE_SUBROUTINE(void,   SetInterfaceManagerCursorAlpha,    0x00583DF0, UInt32 alpha); // alpha out of 255
   //
   DEFINE_SUBROUTINE(void, HideBigFour,          0x0057CC00);
   DEFINE_SUBROUTINE(void, ShowOrRefreshBigFour, 0x0057CAC0);

   float InterfaceManager::GetNormalizedMouseAxisMovement(UInt8 axis) { // 1, 2, 3 = x, y, z
      if (axis < 1)
         return 0.0;
      SInt32 pixels;
      {
         auto input = RE::OSInputGlobals::GetInstance();
         if (!input)
            return 0.0F;
         pixels = CALL_MEMBER_FN(input, GetMouseAxisMovement)(axis);
         if (axis >= 3)
            return (float) pixels;
      }
      float scale;
      {
         if (axis == 1)
            scale = GetNormalizedScreenWidth() / *SInt32ScreenWidth;
         else if (axis == 2)
            scale = -GetNormalizedScreenHeight() / *SInt32ScreenHeight;
      }
      return (float) pixels * scale;
   };
   bool InterfaceManager::OtherMenusAreActive(UInt32 yourMenuID) {
      auto& list = this->activeMenuIDs;
      for (UInt8 i = 0; i < 10; i++) {
         if (list[i] == 0)
            break;
         if (list[i] != yourMenuID)
            return false;
      }
      return true;
   };
   void InterfaceManager::SetCursorPosition(float x, float y, bool forceShowCursor) {
      auto cursor = this->cursor;
      if (!cursor || !cursor->renderedNode)
         return;
      if (forceShowCursor) {
         cursor->renderedNode->m_flags &= ~NiAVObject::kFlag_AppCulled; // clear "culled" flag
         CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_visible, 2.0F);
      }
      x += GetNormalizedScreenMinX(); // Screen center for this system is (0, 0, 0). We need to normalize our screen coordinates, which are top-left-relative.
      y += GetNormalizedScreenMinY(); // 
      NiVector3 position = cursor->renderedNode->m_localTranslate;
      position.x = (std::max)(GetNormalizedScreenMinX() + 1.0F, (std::min)(x, GetNormalizedScreenMaxX() - 2.0F));
      position.y = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_depth) * depthToUnits;
      position.z = (std::max)(GetNormalizedScreenMinY() + 1.0F, (std::min)(-y, GetNormalizedScreenMaxY() - 2.0F));
      cursor->renderedNode->m_localTranslate = position;
      if (position.x != this->cursorPos.x || this->cursorPos.z != position.z) { // non-depth coordinate has changed
         this->cursorPos = position;
         this->unk0B9 = true;
         //
         float sWidth = *SInt32ScreenWidth;
         float sHeight = *SInt32ScreenHeight;
         this->unk02C = (sWidth / GetNormalizedScreenWidth() * this->cursorPos.x) + (sWidth / 2);
         this->unk030 = position.y;
         this->unk034 = (sHeight * 0.5) - (sHeight / GetNormalizedScreenHeight() * this->cursorPos.z);
      }
      CALL_MEMBER_FN((RE::NiAVObject*)cursor->renderedNode, Subroutine00707370)(0.0F, 1);
   };
};