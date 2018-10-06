#include "MapMenu.h"
#include "ReverseEngineered/Forms/TESWorldSpace.h"

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
   float MapMenu::GetLastComputedWorldMapZoom() {
      float width = CALL_MEMBER_FN(this->worldMap, GetFloatTraitValue)(RE::kTagID_width);
      auto  data  = CALL_MEMBER_FN((RE::TESWorldSpace*) this->worldSpace, GetRootWorldspaceUnk84)();
      //
      // this->worldMapWidth == data->width * zoom
      // a = b * c
      // a / b = c
      //
      return width / data->width; // inexact, since (width) was rounded when first computed
   };
   void MapMenu::UpdateWorldMapZoom() {
      //
      // UpdateMapMenuWorldMap performs these steps when MapMenu displays a worldspace 
      // for the first time.
      //
      float new_zoom = CALL_MEMBER_FN(this->worldMap, GetFloatTraitValue)(RE::kTagID_zoom) / 100.0F;
      auto  data = CALL_MEMBER_FN((RE::TESWorldSpace*) this->worldSpace, GetRootWorldspaceUnk84)();
      this->worldMapWidth  = data->width  * new_zoom;
      this->worldMapHeight = data->height * new_zoom;
      CALL_MEMBER_FN(this->worldMap, UpdateFloat)(RE::kTagID_width,  this->worldMapWidth);
      CALL_MEMBER_FN(this->worldMap, UpdateFloat)(RE::kTagID_height, this->worldMapHeight);
   };

   DEFINE_SUBROUTINE(void, UpdateMapMenuLocalMap, 0x005BA4D0, bool);
   DEFINE_SUBROUTINE(void, UpdateMapMenuWorldMap, 0x005B91E0);
};