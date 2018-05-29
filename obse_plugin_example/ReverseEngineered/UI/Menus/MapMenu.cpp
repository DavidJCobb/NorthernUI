#include "MapMenu.h"

namespace RE {
   NiPointer<NiNode>* const niptrLODWaterRoot = (NiPointer<NiNode>*) 0x00B35234;
   BSClearZNode**     const niptrLODRoot = (BSClearZNode**)0x00B333A8;

   float* const mapMenuBackgroundColorR = (float*)0x00A4695C; // used only if the local map is using a background image; used as an argument 
   float* const mapMenuBackgroundColorG = (float*)0x00A46958; // to NiDX9Renderer::SetClearColor4 when rendering each cell to the local map; 
   float* const mapMenuBackgroundColorB = (float*)0x00A46954; // effectively controls the background color (akin to GIMP Color to Alpha).

   void MapMenu::ForceDismissTooltip() {
      //
      // This code is based on the content of MapMenu::HandleMouseout:
      //
      auto tooltip  = this->worldIconPaper;
      auto previous = this->hoveredMapIcon;
      CALL_MEMBER_FN(tooltip, UpdateFloat)(kTileValue_visible, 1.0F);
      CALL_MEMBER_FN(tooltip, UpdateString)(kTileValue_string, "");
      //
      if (previous)
         CALL_MEMBER_FN(previous, UpdateFloat)(kTileValue_user7, 0.0F);
      this->hoveredMapIcon = nullptr;
   };
};