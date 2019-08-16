#include "LoadingMenu.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/INISettings.h"

namespace RE {
   void LoadingMenu::ForceProgress(UInt32 segment, float progress) { // recreates non-member subroutine at 0x005ADEC0
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTagID_visible, 2.0F);
      ThisStdCall(0x0058FBA0, this->tile, 0);
      if (this->displayedProgress < 100)
         ThisStdCall(0x00572F60, *(void**)0x00B3A6B0, 0);
      if (false) { // INI::LoadingBar::bShowSectionTimes
         if (segment != this->currentSegment) {
            _MESSAGE("Loading Bar Section %d", segment);
            *(SInt32*)0x00B3B3EC = 0;
         } else {
            SInt32 eax = progress / 10.0F;
            if (eax > *(SInt32*)0x00B3B3EC) {
               *(SInt32*)0x00B3B3EC = eax;
               _MESSAGE("Loading Bar Section %d %d0%%", segment, eax);
            }
         }
      }
      if (segment < this->currentSegment) {
         _MESSAGE("LoadingMenu sections loading out of order. Trying to use (%d:%d%%) but (%d:%d%%) was our last section.", segment, this->percentage[segment], this->currentSegment, this->percentage[this->currentSegment]);
         this->currentSegment = segment;
         if (segment + 1 < 4) {
            this->percentage[segment + 1] = 0;
            memset(&this->percentage[segment + 1], 0, (4 - segment - 1) * sizeof(UInt32));
         }
      }
      if (this->currentSegment < segment) {
         do {
            if (this->currentSegment != -1)
               this->percentage[this->currentSegment] = 100;
         } while (++this->currentSegment < segment);
      }
      if (progress > this->percentage[segment])
         this->percentage[segment] = progress;
      float esp0C = 0;
      UInt32 i = 0;
      do {
         float s = INI::fLoadingBarSegmentPercentages[i]->f;
         esp0C += s * this->percentage[i];
      } while (++i < 4);
      this->displayedProgress = esp0C;
      // TODO: call sub005AD980(0);
      CALL_MEMBER_FN(this, Subroutine005AD440)(this->unk44);
      CALL_MEMBER_FN(RE::InterfaceManager::GetInstance(), CreateBigFourMenuInstances)();
   }
};