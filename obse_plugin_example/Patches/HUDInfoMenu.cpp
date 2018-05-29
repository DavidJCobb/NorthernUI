#include "Patches/HUDInfoMenu.h"
//
#include "Patches/XboxGamepad/Main.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/HUDInfoMenu.h"
#include "obse/GameMenus.h"
#include "obse/Utilities.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace HUDInfoMenu {
      void GetKeycodes(UInt32* outPC, UInt32* outXbox) {
         auto input = RE::OSInputGlobals::GetInstance();
         UInt32 pc = input->keyboard[RE::kControl_Activate];
         if (pc == 0 || pc == 0xFF) {
            pc = input->mouse[RE::kControl_Activate];
            if (pc == 0xFF)
               pc = 0;
            else
               pc += 0x100; // mouse raw button index to DXScanCode
         }
         UInt32 xbox = 0;
         if (g_xInputPatchApplied)
            xbox = XInputControlBindingToDXScanCode(input->joystick[RE::kControl_Activate]);
         *outPC   = pc;
         *outXbox = xbox;
      };

      UInt32 Inner(RE::TESObjectREFR* target) {
         UInt32 iconEnum = 0;
         auto      menu = (RE::HUDInfoMenu*) *g_HUDInfoMenu;
         RE::Tile* tile = nullptr;
         if (menu)
            tile = (RE::Tile*) menu->tile;
         if (!tile)
            return iconEnum;
         {  // Pass the keymappings for the Activate control, so that we can display them in the UI.
            static UInt32 priorPC   = 0;
            static UInt32 priorXbox = 0;
            UInt32 pc;
            UInt32 xbox;
            GetKeycodes(&pc, &xbox);
            if (pc != priorPC) {
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user17, pc);
               priorPC = pc;
            }
            if (xbox != priorXbox) {
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user18, xbox);
               priorXbox = xbox;
            }
         }
{//DEBUG
   auto pad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
   if (pad) {
      CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user10, pad->GetJoystickAxis(1));
      CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user11, pad->GetJoystickAxis(2));
   }
}
         if (target) {
            /*// The HorseFix hook already handles this.
            if (CALL_MEMBER_FN(target, IsHorse)()) { // Interacting with a ridden horse counts as interacting with its rider.
               auto tActor = (RE::Actor*) target;
               if (tActor->horseOrRider)
                  target = tActor->horseOrRider;
            }*/
            iconEnum         = CALL_MEMBER_FN(target, GetHUDReticleIconEnum)(); // the call that we patched over
            auto base        = target->GetBaseForm();
            bool isActor     = false;
            bool isLiveActor = false;
            {
               float value = 0.0F;
               if (base) {
                  value = base->typeID;
                  if (base->typeID == kFormType_NPC || base->typeID == kFormType_Creature) {
                     isActor = true;
                     isLiveActor = !target->IsDead(0);
                  }
               }
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user19, value); // NorthernUI: Form type
            }
            {
               float value = 1.0F;
               if (!OBLIVION_CAST(target, TESObjectREFR, ArrowProjectile)) // per vanilla code, scavenging a used arrow is never a crime
                  if (CALL_MEMBER_FN(target, ActivatingWouldBeACrime)())
                     value = 2.0F;
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user20, value); // NorthernUI: Activating the object would be a crime?
            }
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user21, (float) iconEnum); // NorthernUI: HUDReticle icon enum
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user22, (float) CALL_MEMBER_FN(target, GetOpenState)());
            {  // Vars related to the reference being a lootable container
               SInt8 v = 0;
               v |= (!isLiveActor && target->IsEmpty()); // flag 0x0001: reference is empty and not a live actor
               v |= (isActor && !isLiveActor) << 1;      // flag 0x0002: reference is a dead actor
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user23, v);
            }
            {  // Vars related to the reference not being interactable
               bool canBeInteracted = true;
               bool canYield = false;
               if (isLiveActor) {  // Whether the player can yield
                  auto tActor = (RE::Actor*) target;
                  auto combat = tActor->GetCombatController();
                  if (combat && !combat->IsCalmed()) {
                     if (CALL_MEMBER_FN(combat, TargetsIncludeActor)((RE::Actor*) *g_thePlayer)) {
                        //
                        // The player is aiming at an actor that is in combat with them. If the player is holding "block," 
                        // then they can yield.
                        //
                        auto input = RE::OSInputGlobals::GetInstance();
                        if (CALL_MEMBER_FN(input, QueryControlState)(RE::kControl_Block, RE::kKeyQuery_Hold)) {
                           canYield = true;
                        } else
                           canBeInteracted = false; // cannot interact with actors you are fighting with, except to yield
                     }
                  }
               }
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user15, canYield ? 2.0F : 1.0F); // &true; if it can be yielded to
               CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user16, canBeInteracted ? 1.0F : 2.0F); // &true; if it CANNOT be interacted with.
            }
         } else {
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user16, 1.0F); // cannot be interacted with (reset to default: false)
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user19, 0.0F); // form type
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user20, 1.0F); // activating would be a crime?
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user23, 1.0F); // is empty and a lootable container?
         }
         return iconEnum;
      };
      __declspec(naked) void HorseFix() {
         //
         // The vanilla game checks if the object you're aiming at is a horse, and 
         // if it's being ridden. If so, it performs certain actions on the rider 
         // instead. However, these checks are made after the game has already 
         // retrieved the horse's name and icon-enum. I want them made before, so 
         // I'm just recreating them almost instruction-for-instruction and using 
         // a hook to force them to run as soon as we know we're aiming at some-
         // thing.
         //
         __asm {
            // ecx already == edi
            //
            // Code here recreates branch at 005A4B4C. We'll end up running the code 
            // redundantly, but oh well.
            //
            // Disposable registers: eax, ecx, edx, esi
            //
            mov  eax, 0x004D74D0; // TESObjectREFR::IsHorse
            call eax;
            test al, al;
            jz   lFinish;
            mov  eax, dword ptr [edi];
            mov  eax, dword ptr [eax + 0x388];
            mov  ecx, edi;
            call eax; // auto eax = edi->Unk_E2(); // presumably "get rider"
            mov  esi, eax;
            test esi, esi;
            jz   lFinish;
            mov  eax, g_thePlayer;
            mov  eax, dword ptr [eax];
            cmp  esi, eax;
            je   lFinish;
            mov  eax, dword ptr [esi];
            mov  eax, dword ptr [eax + 0x198];
            push 0;
            mov  ecx, esi;
            call eax; // bool eax = esi->IsDead(0);
            test al, al;
            jnz  lFinish;
            mov  ecx, dword ptr [esi + 0x58];
            test ecx, ecx;
            jz   lFinish;
            mov  eax, dword ptr [ecx];
            mov  eax, dword ptr [eax + 0x36C];
            call eax; // eax = esi->process->Unk_DB();
            cmp  eax, 4;
            jne  lFinish;
            mov  edi, esi;
         lFinish:
            mov  ecx, edi; // reproduce patched-over call
            mov  eax, 0x004DA2A0; // const char* TESObjectREFR::GetFullName();
            call eax;
            mov  ecx, 0x005A49D8; // exit
            jmp  ecx;
         };
      };
      __declspec(naked) void Outer() {
         __asm {
            push ecx;    //
            call Inner;  //
            add  esp, 4; // auto eax = Inner(edi);
            mov  ecx, 0x005A4AE0; //
            jmp  ecx;             // // keep eax; the code we return to will use it
         };
      };

      void Apply() {
         WriteRelJump(0x005A4ADB, (UInt32)&Outer); // patch over a call to TESObjectREFR::GetHUDReticleIconEnum
         WriteRelJump(0x005A49D3, (UInt32)&HorseFix);
      };
   };
};