#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/Menu.h"
#include "obse/GameExtraData.h"
#include "obse/GameMenus.h"
#include "obse/NiTypes.h"
#include "obse/Utilities.h"
#include "shared.h"

namespace RE {
   DEFINE_SUBROUTINE_EXTERN(bool, AreImagespaceEffectsAvailable, 0x005DDD00); // checks INI setting and hardware support for post pixel shader blending

   class VideoMenu : public RE::Menu {
      public:
         enum { kID = 0x3FA };

         struct VideoRes { // sizeof == 0x10; contents unknown
            UInt32 width;
            UInt32 height;
            UInt32 unk08;
            UInt32 unk0C;
         };
         struct NiTList_VideoRes {
            public:
               struct Node {
                  Node* next;
                  Node* prev;
                  VideoRes data;
               };
               virtual void  Dispose(bool);
               virtual Node* AllocateNode();
               virtual void  FreeNode(Node* node);
               //
               Node*  start;    // 04
               Node*  end;      // 08
               UInt32 numItems; // 0C
         };

         enum {
            kTileID_ButtonRestoreDefaults = 4,
            kTileID_ButtonReturn          = 1,
            kTileID_OptionsScrollbar             =  2,
            kTileID_OptionsScrollbarThumb        =  3,
            kTileID_OptionsPane                  =  7,
            kTileID_OptionActorLODSlider         = 14,
            kTileID_OptionActorLODSliderThumb    = 15,
            kTileID_OptionAntialiasEnumButtonLeft  = 45,
            kTileID_OptionAntialiasEnumButtonRight = 46,
            kTileID_OptionAntialiasKeynavShim    = 44,
            kTileID_OptionBloodButton            = 43,
            kTileID_OptionBloomButton            = 47,
            kTileID_OptionCanopyShadowsButton    = 11,
            kTileID_OptionDistantBuildingsButton = 30,
            kTileID_OptionDistantLandButton      =  9,
            kTileID_OptionDistantTreesButton     = 31,
            kTileID_OptionGammaSlider          =  5,
            kTileID_OptionGammaSliderThumb     =  6,
            kTileID_OptionGrassShadowsButton   = 21,
            kTileID_OptionGrassSlider          = 22,
            kTileID_OptionGrassSliderThumb     = 23,
            kTileID_OptionHDRButton            = 38,
            kTileID_OptionItemLODKeynavShim    = 24,
            kTileID_OptionItemLODSlider        = 16,
            kTileID_OptionItemLODSliderThumb   = 17,
            kTileID_OptionObjectLODKeynavShim  = 25,
            kTileID_OptionObjectLODSlider      = 18,
            kTileID_OptionObjectLODSliderThumb = 19,
            kTileID_OptionResolutionEnumButtonLeft    =  8,
            kTileID_OptionResolutionEnumButtonRight   = 26,
            kTileID_OptionResolutionKeynavShim        = 27,
            kTileID_OptionSelfShadowsButton           = 10,
            kTileID_OptionShadowFilteringButton       = 48,
            kTileID_OptionShadowsExteriorSlider       = 34,
            kTileID_OptionShadowsExteriorSliderThumb  = 35,
            kTileID_OptionShadowsInteriorSlider       = 32,
            kTileID_OptionShadowsInteriorSliderThumb  = 33,
            kTileID_OptionSpecularDistanceSlider      = 36,
            kTileID_OptionSpecularDistanceSliderThumb = 37,
            kTileID_OptionTextureSizeButton       = 20,
            kTileID_OptionTreeLODSlider           = 12,
            kTileID_OptionTreeLODSliderThumb      = 13,
            kTileID_OptionViewDistanceSlider      = 28,
            kTileID_OptionViewDistanceSliderThumb = 29,
            kTileID_OptionWaterDetailButton       = 39,
            kTileID_OptionWaterDisplacementButton = 41,
            kTileID_OptionWaterReflectionsButton  = 40,
            kTileID_OptionWindowReflectionsButton = 42,
            kTileID_Count = 0x30,
         };
         Tile*  tiles[kTileID_Count]; // 028 // tiles[(SInt32)tileID - 1]
         UInt32 unk0E8 = 0; // 0E8
         UInt32 unk0EC; // 0EC
         UInt32 bDecalsOnSkinnedGeometry; // 0F0 // gets set to !!INI:Display:bDecalsOnSkinnedGeometry + 1
         UInt32 unk0F4; // 0F4
         UInt32 bUseWaterHiRes; // 0F8 // gets set to !!INI:Water:bUseWaterHiRes
         UInt32 unk0FC; // 0FC // gets set to: []() { a = INI:Display:iMultiSample; c = (a <= 1) + 1; return a & c; }
         NiTList_VideoRes resolutions; // 100 // see code starting at 005DEE68 in ShowVideoMenu
         NiTList_VideoRes::Node* unk110;
         UInt8  unk114;
         UInt8  unk115; // 115
         UInt8  unk116;
         UInt8  unk117;

         void ResetDefaults();

         inline Tile* getTile(UInt32 a) {
            return this->tiles[a - 1];
         }

         MEMBER_FN_PREFIX(VideoMenu);
         DEFINE_MEMBER_FN(Subroutine005DE2E0, void, 0x005DE2E0);
         DEFINE_MEMBER_FN(Subroutine005DE920, void, 0x005DE920);
         DEFINE_MEMBER_FN(Subroutine005DEAD0, void, 0x005DEAD0);
   };
};