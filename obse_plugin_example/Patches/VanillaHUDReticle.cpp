#include "VanillaHUDReticle.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/UI/General.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/HUDReticle.h"
#include "obse/GameObjects.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace HUDReticle {
      namespace AllowThirdPerson {
         void Shim(UInt8 state) {
            auto tile1 = (::Tile*) *RE::HUDReticleTileID1;
            if (tile1) {
               float x;
               if (tile1->GetFloatValue(kTileValue_user20, &x)) {
                  if (x == 1.0F) {
                     state = 1; // force-show if our XML has disabled hiding the reticle in third-person
                     if (*RE::bPlayerInVanityMode || *(bool*)(0x00B3BB05) || RE::IsMenuMode())
                        state = 0;
                  }
               }
            }
            RE::SetHUDReticleStateIfPlayerHasNode(state); // the original call we patched over
         };
         void Apply() {
            WriteRelCall(0x00673442, (UInt32)&Shim);
         };
      }
      namespace DetailedSneak {
         namespace OpacityControls { // forward-declare
            void UpdateInner();
         };

         void Inner(RE::Tile* tile, bool isSneak, UInt32 traitID, float initial, float target, float time) {
            if (tile == nullptr)
               return;
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user20, isSneak ? 2.0F : 1.0F);
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user21, target);
            CALL_MEMBER_FN(tile, AnimateTrait)(kTileValue_user8, initial, target, time);
            {  // Allow us to do our own animations as desired
               float x = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_user19);
               CALL_MEMBER_FN(tile, AnimateTrait)(kTileValue_user19, x, isSneak ? 1.0F : 0.0F, time);
            }
            //
            OpacityControls::UpdateInner(); // for now we don't have a good hook site for this, so update it here and the game will use it next alpha update
         };
         __declspec(naked) void OuterSneak(UInt32 traitID, float initial, float target, float time) {
            __asm {
               sub  esp, 0x10;
               push 1;
               push ecx;
               // &traitID == esp + 1C
               mov  eax, dword ptr [esp + 0x1C];
               mov  ecx, dword ptr [esp + 0x20];
               mov  dword ptr [esp + 0x08], eax;
               mov  dword ptr [esp + 0x0C], ecx;
               mov  eax, dword ptr [esp + 0x24];
               mov  ecx, dword ptr [esp + 0x28];
               mov  dword ptr [esp + 0x10], eax;
               mov  dword ptr [esp + 0x14], ecx;
               call Inner;
               add esp, 0x18;
               retn 0x10;
            };
         };
         __declspec(naked) void OuterNoSneak(UInt32 traitID, float initial, float target, float time) {
            __asm {
               sub  esp, 0x10;
               push 0;
               push ecx;
               // &traitID == esp + 1C
               mov  eax, dword ptr [esp + 0x1C];
               mov  ecx, dword ptr [esp + 0x20];
               mov  dword ptr [esp + 0x08], eax;
               mov  dword ptr [esp + 0x0C], ecx;
               mov  eax, dword ptr [esp + 0x24];
               mov  ecx, dword ptr [esp + 0x28];
               mov  dword ptr [esp + 0x10], eax;
               mov  dword ptr [esp + 0x14], ecx;
               call Inner;
               add esp, 0x18;
               retn 0x10;
            };
         };

         namespace OpacityControls {
            static float fOpacityLost    =  50.0F;
            static float fOpacityUnseen  =  50.0F;
            static float fOpacityNoticed =  50.0F;
            static float fOpacitySeen    = 200.0F;
            //
            void UpdateInner() {
               //
               // For now, we don't have a good hook site for this, so we just have the DetailedSneak 
               // hook call it (and then the alpha values actually get used next time that hook runs).
               //
               auto sneak = *RE::HUDReticleTileID3;
               if (sneak) {
                  //
                  // Overwrite these floats only if the trait is defined. Don't bother 
                  // checking the result bool (indicating trait existence): the float 
                  // is not overwritten if the trait isn't defined.
                  //
                  sneak->GetFloatValue(kTileValue_user22, &fOpacityLost);
                  sneak->GetFloatValue(kTileValue_user23, &fOpacityUnseen);
                  sneak->GetFloatValue(kTileValue_user24, &fOpacityNoticed);
                  sneak->GetFloatValue(kTileValue_user25, &fOpacitySeen);
               }
            };
            //
            void Patch() {
               SafeWrite32(0x005A80F4, (UInt32) &fOpacityLost);
               SafeWrite32(0x005A8146, (UInt32) &fOpacityUnseen);
               SafeWrite32(0x005A8196, (UInt32) &fOpacityNoticed);
               SafeWrite32(0x005A81E8, (UInt32) &fOpacitySeen);
            };
         };

         void Apply() {
            WriteRelCall(0x005A838E, (UInt32) &OuterSneak);
            WriteRelCall(0x005A83DA, (UInt32) &OuterNoSneak);
            WriteRelCall(0x005A8854, (UInt32) &OuterSneak);
            WriteRelCall(0x005A8880, (UInt32) &OuterNoSneak);
            WriteRelCall(0x005A894C, (UInt32) &OuterNoSneak);
            OpacityControls::Patch();
         };
      };

      void Apply() {
         AllowThirdPerson::Apply(); // handles reticle visibility when the player switches in or out of third-person
         DetailedSneak::Apply();
      }
   }
};