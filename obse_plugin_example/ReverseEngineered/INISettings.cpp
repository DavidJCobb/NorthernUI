#include "INISettings.h"

namespace RE {
   bool* const bShouldCreateShaderPackage = (bool*) 0x00B430AC;
   bool* const bShouldDisplayLODLand = (bool*) 0x00B43077;
   bool* const bShouldDisplayDynamicWindowReflections = (bool*) 0x00B430AD;
   IniSettingCollection* const g_IniSettingCollection = (IniSettingCollection*) 0x00B07BF0;

   namespace INI {
      DEFINE_SUBROUTINE(void, SetSpecularDistances, 0x00497D20, float);

      namespace Audio {
         SettingInfo* const fDefaultEffectsVolume = (SettingInfo*) 0x00B161A8;
         SettingInfo* const fDefaultFootVolume    = (SettingInfo*) 0x00B16198;
         SettingInfo* const fDefaultMasterVolume  = (SettingInfo*) 0x00B16190;
         SettingInfo* const fDefaultMusicVolume   = (SettingInfo*) 0x00B161A0;
         SettingInfo* const fDefaultVoiceVolume   = (SettingInfo*) 0x00B161B0;
      };
      namespace BlurShader {
         SettingInfo* const bUseBlurShader = (SettingInfo*) 0x00B06D34; // default value is !TES4_005DDD00();
      };
      namespace BlurShaderHDR {
         SettingInfo* const bDoHighDynamicRange = (SettingInfo*) 0x00B06DE4; // default value is !!TES4_005DDD00();
      };
      namespace Controls {
         SettingInfo* const fJoystickMoveFBMult = (SettingInfo*) 0x00B14EF0;
         SettingInfo* const fJoystickMoveLRMult = (SettingInfo*) 0x00B14EF8;
         SettingInfo* const iJoystickLookLeftRight = (SettingInfo*) 0x00B14EE0;
         SettingInfo* const iJoystickLookUpDown    = (SettingInfo*) 0x00B14ED8;
         SettingInfo* const iJoystickMoveLeftRight = (SettingInfo*) 0x00B14ED0;
         SettingInfo* const iJoystickMoveFrontBack = (SettingInfo*) 0x00B14EC8;
      };
      namespace Display {
         SettingInfo* const bActorSelfShadowing  = (SettingInfo*) 0x00B06F0C; // default value is false
         SettingInfo* const bDoCanopyShadowPass  = (SettingInfo*) 0x00B06CBC; // default value is true
         SettingInfo* const bDoImageSpaceEffects = (SettingInfo*) 0x00B06CC4;
         SettingInfo* const bShadowsOnGrass      = (SettingInfo*) 0x00B06F14; // default value is false
         //
         SettingInfo* const fNoLODFarDistancePct = (SettingInfo*) 0x00B0312C; // default value is 1.0F
         //
         SettingInfo* const iActorShadowCountExt = (SettingInfo*) 0x00B06EFC; // default value is 2
         SettingInfo* const iActorShadowCountInt = (SettingInfo*) 0x00B06F04; // default value is 4
         SettingInfo* const iMultiSample         = (SettingInfo*) 0x00B06D0C;
         SettingInfo* const iTexMipMapSkip       = (SettingInfo*) 0x00B06D24;
      };
      namespace Gameplay { // actually "GamePlay" but screw that
         SettingInfo* const bCrosshair         = (SettingInfo*) 0x00B13210;
         SettingInfo* const bDialogueSubtitles = (SettingInfo*) 0x00B13200;
         SettingInfo* const bDisableDynamicCrosshair = (SettingInfo*) 0x00B13238;
         SettingInfo* const bGeneralSubtitles  = (SettingInfo*) 0x00B13208;
         SettingInfo* const bSaveOnInteriorExteriorSwitch = (SettingInfo*) 0x00B13230;
         SettingInfo* const bSaveOnRest        = (SettingInfo*) 0x00B13218;
         SettingInfo* const bSaveOnTravel      = (SettingInfo*) 0x00B13228;
         SettingInfo* const bSaveOnWait        = (SettingInfo*) 0x00B13220;
      };
      namespace General {
         SettingInfo* const bBorderRegionsEnabled = (SettingInfo*) 0x00B14F58;
      };
      namespace Grass {
         SettingInfo* const fGrassEndDistance = (SettingInfo*) 0x00B09B18; // default value is 3000.0F
      };
      namespace Interface {
         SettingInfo* const fKeyRepeatInterval = (SettingInfo*) 0x00B135B8;
         SettingInfo* const fKeyRepeatTime     = (SettingInfo*) 0x00B135B0;
      };
      namespace LOD {
         SettingInfo* const bDisplayLODBuildings = (SettingInfo*) 0x00B09AE8; // default value is true
         SettingInfo* const bDisplayLODLand      = (SettingInfo*) 0x00B02D70;
         SettingInfo* const bDisplayLODTrees     = (SettingInfo*) 0x00B09AF0; // default value is true
         //
         SettingInfo* const fActorLODDefault       = (SettingInfo*) 0x00B14814;
         SettingInfo* const fItemLODDefault        = (SettingInfo*) 0x00B1482C;
         SettingInfo* const fObjectLODDefault      = (SettingInfo*) 0x00B14844;
         SettingInfo* const fTreeLODDefault        = (SettingInfo*) 0x00B147FC;
         SettingInfo* const fLODFadeOutMultActors  = (SettingInfo*) 0x00B0762C; // default value is in fActorLODDefault
         SettingInfo* const fLODFadeOutMultItems   = (SettingInfo*) 0x00B07624; // default value is in fItemLODDefault
         SettingInfo* const fLODFadeOutMultObjects = (SettingInfo*) 0x00B0761C; // default value is in fObjectLODDefault
         SettingInfo* const fLODMultTrees          = (SettingInfo*) 0x00B0760C; // default value is in fTreeLODDefault
      };
      namespace Menu {
         SettingInfo* const iDebugTextFont = (SettingInfo*) 0x00B12DB4;
      };
      namespace Water {
         SettingInfo* const bUseWaterDisplacements = (SettingInfo*) 0x00B07090; // default value is true
         SettingInfo* const bUseWaterReflections   = (SettingInfo*) 0x00B07060; // default value is true
      };
   };
};