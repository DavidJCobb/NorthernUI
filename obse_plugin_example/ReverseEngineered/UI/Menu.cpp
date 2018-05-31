#include "Menu.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameAPI.h"
#include "obse/Utilities.h"

#include <algorithm>
using namespace std;

namespace RE {
   DEFINE_SUBROUTINE(UInt8, GetAndResetMessageButtonPressed, 0x00578D70);
   //
   Menu::Menu() {
      ThisStdCall(0x00584640, this);
   };
   Menu::~Menu() {
      ThisStdCall(0x00585330, this);
   };
   void Menu::Dispose(bool free) {
      this->~Menu();
      if (free)
         FormHeap_Free(this);
   };
   void Menu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {};
   void Menu::HandleMouseDown(SInt32 tileID, RE::Tile* target) {};
   void Menu::HandleMouseUp(SInt32 tileID, RE::Tile* target) {};
   void Menu::HandleMouseover(SInt32 tileID, RE::Tile* target) {};
   void Menu::HandleMouseout(SInt32 tileID, RE::Tile* target) {};
   void Menu::Unk_06(SInt32 tileID, RE::Tile* tileMain, RE::Tile* tileOther) {};
   void Menu::Unk_07(SInt32 tileID, RE::Tile* tileMain, RE::Tile* tileOther) {};
   void Menu::HandleFrameMouseDown(SInt32 tileID, RE::Tile* target) {};
   void Menu::Unk_09(SInt32 tileID, RE::Tile* target) {};
   void Menu::HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target) {};
   void Menu::HandleFrame() {};
   bool Menu::HandleKeyboardInput(UInt32 inputChar) { return false; };
   UInt32 Menu::GetID() { return 0; };
   bool Menu::HandleNavigationInput(NavigationInput argEnum, float arg1) { return false; };

   void Menu::Close() {
      if (!this->tile)
         return;
      CALL_MEMBER_FN(this->tile, UpdateFloat)(0x1772, 2.0F);
      CALL_MEMBER_FN(this, FadeOut)(); // only after setting 1772? only for some menus?
      //
      // The above represents the absolute minimum instructions for closing a modal menu. Additional code handles 
      // closing a menu that was opened as a submenu of the "big four:"
      //
      if (RE::GetTopmostMenuIDUnderUnkCondition() == 1)
         RE::Subroutine0057CAC0();
   };
   void Menu::HandleFocusBox(Tile* focusBox, Tile* target) {
      // based on AlchemyMenu's mouseover handler
      if (target) {
         auto inset = CALL_MEMBER_FN(target, GetFloatTraitValue)(kTileValue_focusinset);
         float targetX, targetY, targetZ;
         target->GetAbsoluteCoordinates(targetX, targetY, targetZ);
         float depth = targetZ - CALL_MEMBER_FN(focusBox, GetFloatTraitValue)(kFocusBoxTraitValue_DepthBackwardOffset);
         {
            float offsetX, offsetY, offsetZ;
            auto container = focusBox->parent;
            if (container) {
               container->GetAbsoluteCoordinates(offsetX, offsetY, offsetZ);
               targetX -= offsetX;
               targetY -= offsetY;
               depth   -= offsetZ;
            }
         }
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_depth, depth);
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_width,  CALL_MEMBER_FN(target, GetFloatTraitValue)(kTileValue_width)  - (inset * 2));
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_height, CALL_MEMBER_FN(target, GetFloatTraitValue)(kTileValue_height) - (inset * 2));
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_x, targetX + inset);
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_y, targetY + inset);
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_visible, 2.0F);
      } else {
         CALL_MEMBER_FN(focusBox, UpdateFloat)(kTileValue_visible, 1.0F);
      }
   };
   void Menu::HandleScrollbarThumbDrag(Tile* bar, Tile* thumb, ScrollDragBehavior behavior, bool isHorizontal) {
      float offset;
      {
         auto ui = RE::InterfaceManager::GetInstance();
         if (isHorizontal) {
            float a = (RE::GetNormalizedScreenWidth() / 2) + ui->cursorPos.x;
            offset = ((SInt32)a) - CALL_MEMBER_FN(bar, GetAbsoluteXCoordinate)();
         } else {
            float a = (RE::GetNormalizedScreenHeight() / 2) - ui->cursorPos.z;
            offset = ((SInt32)a) - CALL_MEMBER_FN(bar, GetAbsoluteYCoordinate)();
         }
      }
      //
      Tile*  target;
      UInt16 trait;
      float  final;
      switch (behavior) {
         //
         // I mostly understand the differences in these behaviors' mechanisms of action, but as of this 
         // writing I don't yet understand what visible differences these behaviors are meant to produce, 
         // or why Bethesda chose to offer more than one behavior.
         //
         default:
         case kScrollDragBehavior_Interval:
            {  // Based on AudioMenu, DialogMenu, and InventoryMenu drag behavior.
               // Seems to scroll based on pixels from the scrollbar track top.
               //
               // We don't clamp the scroll position here because we aren't directly modifying it. Instead, 
               // we're (more-or-less) telling the scrollbar thumb how much we want to scroll by, and math 
               // in the thumb's own XML receives that value and updates the scroll position itself. Among 
               // other things, this means that we don't need to clamp the scroll value to the scrollbar's 
               // specified minimum or maximum; the thumb's XML already does that.
               //
               final = offset / CALL_MEMBER_FN(thumb, GetFloatTraitValue)(RE::kScrollThumbTraitValue_DragInterval);
               trait = kTileValue_user9;
               target = thumb;
            }
            break;
         case kScrollDragBehavior_ClickStep:
            {  // Based on MapMenu drag behavior.
               // Seems to scroll based on pixels from the scrollbar track top.
               //
               // We don't clamp the scroll position here because we aren't directly modifying it. Instead, 
               // we're (more-or-less) telling the scrollbar thumb how much we want to scroll by, and math 
               // in the thumb's own XML receives that value and updates the scroll position itself. Among 
               // other things, this means that we don't need to clamp the scroll value to the scrollbar's 
               // specified minimum or maximum; the thumb's XML already does that.
               //
               // I don't yet understand how this is meant to differ from the "Interval" behavior.
               //
               final = offset / CALL_MEMBER_FN(thumb, GetFloatTraitValue)(RE::kScrollThumbTraitValue_DragInterval);
               final /= CALL_MEMBER_FN(bar, GetFloatTraitValue)(RE::kScrollbarTraitValue_ArrowClickStep); // ?!?!?!
               trait = kTileValue_user9;
               target = thumb;
            }
            break;
         case kScrollDragBehavior_Advanced:
            {  // Based on StatsMenu drag behavior (same for all three scrollbars).
               // Seems to scroll based on percentage position within the scrollbar track.
               // 
               // By modifying the initial scroll position, we avoid the "manual step" and "drag interval" 
               // prefs: we bypass the scrollbar's XML-side math and guarantee that the scroll value WILL 
               // BE the nearest scroll value to the cursor's percentage offset within the scrollbar track. 
               // This is also why we retrieve the min/max scroll values here but not for the other cases.
               //
               float minScroll = CALL_MEMBER_FN(bar, GetFloatTraitValue)(kScrollbarTraitValue_MinScroll);
               float maxScroll = CALL_MEMBER_FN(bar, GetFloatTraitValue)(kScrollbarTraitValue_MaxScroll);
               float percentage = offset / CALL_MEMBER_FN(bar, GetFloatTraitValue)(isHorizontal ? kTileValue_width : kTileValue_height);
               if (percentage < 0.0F)
                  percentage = 0.0F;
               else if (percentage > 1.0F)
                  percentage = 1.0F;
               final = ((maxScroll - minScroll) * percentage) + minScroll;
               trait = kScrollbarTraitValue_InitialScroll;
               target = bar;
            }
            break;
      }
      CALL_MEMBER_FN(target, UpdateFloat)(trait, -1000000000.0F);
      CALL_MEMBER_FN(target, UpdateFloat)(trait, (SInt32) final);
      CALL_MEMBER_FN(target, UpdateFloat)(trait, 0.0F);
   };
   void Menu::HandleScrollbarMouseWheel(Tile* thumb) {
      auto   ui = RE::InterfaceManager::GetInstance();
      SInt32 y  = ui->mouseZAxisMovement;
      y /= -44; // MSVC optimized the constant Bethesda used; I'm not at all sure what it is. See RepairMenu::HandleFrameScrollWheel for the original code
      //
      CALL_MEMBER_FN(thumb, UpdateFloat)(kTileValue_user9, y);
      CALL_MEMBER_FN(thumb, UpdateFloat)(kTileValue_user9, 0.0F);
   };
};