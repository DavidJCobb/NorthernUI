#include "VideoMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "ReverseEngineered/INISettings.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   DEFINE_SUBROUTINE(bool, AreImagespaceEffectsAvailable, 0x005DDD00);
   DEFINE_SUBROUTINE(void, Subroutine00497AE0, 0x00497AE0, float);
   DEFINE_SUBROUTINE(void, Subroutine00497C30, 0x00497C30);

   void VideoMenu::ResetDefaults() {
      auto menu = (RE::Menu*) this;
      {
         auto tile = (RE::Tile*) this->tiles[kTileID_OptionDistantLandButton - 1];
         if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_target) >= 2.0F) {
            *RE::bShouldDisplayLODLand = true;
            RE::INI::LOD::bDisplayLODLand->b = true;
            CALL_MEMBER_FN(menu, ToggleOnOffLabel)(tile, true);
         }
      }
      RE::INI::LOD::fLODFadeOutMultActors->f = RE::INI::LOD::fActorLODDefault->f;
      RE::INI::LOD::fLODFadeOutMultItems->f = RE::INI::LOD::fItemLODDefault->f;
      RE::INI::LOD::fLODFadeOutMultObjects->f = RE::INI::LOD::fObjectLODDefault->f;
      RE::INI::LOD::fLODMultTrees->f = RE::INI::LOD::fTreeLODDefault->f;
      RE::INI::Display::bActorSelfShadowing->b = false;
      RE::INI::Display::bShadowsOnGrass->b = false;
      RE::INI::Grass::fGrassEndDistance->f = 3000.0F;
      RE::INI::Display::bDoCanopyShadowPass->b = true;
      RE::INI::BlurShaderHDR::bDoHighDynamicRange->b = !!AreImagespaceEffectsAvailable();
      RE::INI::Water::bUseWaterReflections->b = true;
      RE::INI::LOD::bDisplayLODBuildings->b = true;
      RE::INI::LOD::bDisplayLODTrees->b = true;
      RE::INI::Water::bUseWaterDisplacements->b = true;
      *RE::bShouldDisplayDynamicWindowReflections = true;
      RE::INI::BlurShader::bUseBlurShader->b = !AreImagespaceEffectsAvailable();
      //
      Subroutine00497AE0(1.0F);
      Subroutine00497C30();
      {  // Update button states.
         RE::Tile* tile;
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionSelfShadowsButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOffButtonText->s);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionGrassShadowsButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOffButtonText->s);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionCanopyShadowsButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOnButtonText->s);
         //
         CALL_MEMBER_FN(menu, ToggleOnOffLabel)((RE::Tile*) this->tiles[kTileID_OptionHDRButton - 1], RE::INI::BlurShaderHDR::bDoHighDynamicRange->b);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionWaterReflectionsButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOnButtonText->s);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionDistantBuildingsButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOnButtonText->s);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionDistantTreesButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOnButtonText->s);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionWaterDisplacementButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOnButtonText->s);
         //
         tile = (RE::Tile*) this->tiles[kTileID_OptionWindowReflectionsButton - 1];
         if (tile)
            CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, RE::GMST::sOnButtonText->s);
         //
         CALL_MEMBER_FN(menu, ToggleOnOffLabel)((RE::Tile*) this->tiles[kTileID_OptionBloomButton - 1], RE::INI::BlurShader::bUseBlurShader->b);
      }
      {  // Replicates from 005E006B to 005E00AB
         NiTList_VideoRes::Node* found = nullptr;
         for (auto node = this->resolutions.start; node; node = node->next) {
            if (node->data.width != 640)
               continue;
            if (node->data.height == 480) {
               found = node;
               break;
            }
         }
         this->unk110 = found ? found : this->resolutions.start;
         CALL_MEMBER_FN(this, Subroutine005DEAD0)();
      }
      RE::INI::Display::iActorShadowCountExt->i = 2;
      RE::INI::Display::iActorShadowCountInt->i = 4;
      RE::INI::SetSpecularDistances(500.0F);
      RE::INI::Display::fNoLODFarDistancePct->f = 1.0F;
      this->bDecalsOnSkinnedGeometry = 2;
      this->unk0F4 = 0;
      this->bUseWaterHiRes = 0;
      this->unk0FC = 0;
      CALL_MEMBER_FN(this, Subroutine005DE920)();
      CALL_MEMBER_FN(this, Subroutine005DE2E0)();
   };
};