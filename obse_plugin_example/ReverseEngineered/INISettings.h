#pragma once
#include "obse/GameAPI.h"
#include "shared.h"

namespace RE {
   extern bool* const bShouldCreateShaderPackage; // cached value of [General]bCreateShaderPackage
   extern bool* const bShouldDisplayLODLand;
   extern bool* const bShouldDisplayDynamicWindowReflections; // cached value of [Display]bDynamicWindowReflections
   extern IniSettingCollection* const g_IniSettingCollection;

   namespace INI {
      DEFINE_SUBROUTINE_EXTERN(void, SetSpecularDistances, 0x00497D20, float); // sets both specular-LOD distances and updates cached values used at run-time

      namespace Audio {
         extern SettingInfo* const fDefaultEffectsVolume;
         extern SettingInfo* const fDefaultFootVolume;
         extern SettingInfo* const fDefaultMasterVolume;
         extern SettingInfo* const fDefaultMusicVolume;
         extern SettingInfo* const fDefaultVoiceVolume;
      };
      namespace BlurShader {
         extern SettingInfo* const bUseBlurShader; // default value is !TES4_005DDD00();
      };
      namespace BlurShaderHDR {
         extern SettingInfo* const bDoHighDynamicRange; // default value is !!TES4_005DDD00();
      };
      namespace Controls {
         extern SettingInfo* const fJoystickMoveFBMult;
         extern SettingInfo* const fJoystickMoveLRMult;
         extern SettingInfo* const iJoystickLookLeftRight;
         extern SettingInfo* const iJoystickLookUpDown;
         extern SettingInfo* const iJoystickMoveLeftRight;
         extern SettingInfo* const iJoystickMoveFrontBack;
      };
      namespace Display {
         extern SettingInfo* const bActorSelfShadowing; // default value is false
         extern SettingInfo* const bDoCanopyShadowPass; // default value is true
         extern SettingInfo* const bShadowsOnGrass;     // default value is false
         //
         extern SettingInfo* const fNoLODFarDistancePct; // default value is 1.0F
         //
         extern SettingInfo* const iActorShadowCountExt; // default value is 2
         extern SettingInfo* const iActorShadowCountInt; // default value is 4
         extern SettingInfo* const iMultiSample;
         extern SettingInfo* const iTexMipMapSkip; // value cached at 0x00B4205C on startup; pulled from there // texture quality option works by skipping mipmaps; causes corrupted rendering for DDSes with no mipmaps
      };
      namespace Gameplay { // actually "GamePlay" but screw that
         extern SettingInfo* const bCrosshair;
         extern SettingInfo* const bDialogueSubtitles;
         extern SettingInfo* const bDisableDynamicCrosshair;
         extern SettingInfo* const bGeneralSubtitles;
         extern SettingInfo* const bSaveOnInteriorExteriorSwitch;
         extern SettingInfo* const bSaveOnRest;
         extern SettingInfo* const bSaveOnTravel;
         extern SettingInfo* const bSaveOnWait;
      };
      namespace General {
         extern SettingInfo* const bBorderRegionsEnabled;
      };
      namespace Grass {
         extern SettingInfo* const fGrassEndDistance; // default value is 3000.0F
      };
      namespace Interface {
         extern SettingInfo* const fKeyRepeatInterval;
         extern SettingInfo* const fKeyRepeatTime;
      };
      namespace LOD {
         extern SettingInfo* const bDisplayLODBuildings; // default value is true
         extern SettingInfo* const bDisplayLODLand;      // default value is true // if changing this, also change (bool*)(0x00B43077) which is the runtime value
         extern SettingInfo* const bDisplayLODTrees;     // default value is true
         //
         extern SettingInfo* const fActorLODDefault;
         extern SettingInfo* const fItemLODDefault;
         extern SettingInfo* const fObjectLODDefault;
         extern SettingInfo* const fTreeLODDefault;
         extern SettingInfo* const fLODFadeOutMultActors;  // default value is in fActorLODDefault
         extern SettingInfo* const fLODFadeOutMultItems;   // default value is in fItemLODDefault
         extern SettingInfo* const fLODFadeOutMultObjects; // default value is in fObjectLODDefault
         extern SettingInfo* const fLODMultTrees;          // default value is in fTreeLODDefault
      };
      namespace Menu {
         extern SettingInfo* const iDebugTextFont;
      };
      namespace Water {
         extern SettingInfo* const bUseWaterDisplacements; // default value is true
         extern SettingInfo* const bUseWaterReflections;   // default value is true
      };
   };
};