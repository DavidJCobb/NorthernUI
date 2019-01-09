#include "Miscellaneous.h"
#include "obse_common/SafeWrite.h"
#include "Services/INISettings.h"
#include "Patches/TagIDs/Main.h"
#include "ReverseEngineered/ExtraData/ExtraContainerChanges.h"
#include "ReverseEngineered/NetImmerse/NiObject.h"
#include "ReverseEngineered/NetImmerse/NiProperty.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/ContainerMenu.h"
#include "obse/Script.h"
#include "obse/GameMenus.h"

#include <dds.h>
using namespace DirectX;

namespace CobbPatches {
   namespace Miscellaneous {
      namespace Tile3DRenderChanges {
         namespace KeepNiPropertiesInTile3D {
            float uiFalse = 1.0F;
            float uiTrue  = 2.0F;
            void Inner(RE::Tile* tile, RE::NiAVObject* node, NiProperty** out, UInt32 propertyType) {
//_MESSAGE("Importing 3D content for tile %08X - tampering with a NiProperty of type %d...", tile, propertyType);
//if (tile && tile->name.m_data) _MESSAGE(" - Tile name is %s", std::string(tile->name.m_data, 64).c_str());
               if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(CobbPatches::TagIDs::_traitKeepNiProperties) == 2.0F) {
//_MESSAGE(" - Property will be protected.", propertyType);
                  *out = nullptr;
                  return;
               }
//_MESSAGE(" - Property will not be protected.");
               CALL_MEMBER_FN(node, RemoveAndReturnProperty)(out, propertyType);
//_MESSAGE(" - Property removed.");
            };

            __declspec(naked) void Outer1() { // protect NiMaterialProperty (1 of 2)
               _asm {
                  push ecx;
                  mov  ecx, dword ptr [esp + 0x1C]; // == Tile3D* this
                  push ecx;
                  call Inner;
                  add  esp, 0x10;
                  mov  eax, 0x0059082C;
                  jmp  eax;
               }
            };
            __declspec(naked) void Outer2() { // protect NiAlphaProperty (1 of 2)
               _asm {
                  push ecx;
                  mov  ecx, dword ptr[esp + 0x1C]; // == Tile3D* this
                  push ecx;
                  call Inner;
                  add  esp, 0x10;
                  mov  eax, 0x00590860;
                  jmp  eax;
               }
            };
            __declspec(naked) void Outer3() { // protect NiMaterialProperty (2 of 2)
               _asm {
                  push ecx;
                  mov  ecx, dword ptr [esp + 0x20]; // == Tile3D* this
                  push ecx;
                  call Inner;
                  add  esp, 0x10;
                  mov  eax, 0x005908DA;
                  jmp  eax;
               }
            };
            __declspec(naked) void Outer4() { // protect NiAlphaProperty (2 of 2)
               _asm {
                  push ecx;
                  mov  ecx, dword ptr[esp + 0x20]; // == Tile3D* this
                  push ecx;
                  call Inner;
                  add  esp, 0x10;
                  mov  eax, 0x0059090E;
                  jmp  eax;
               }
            };
            bool InnerNoCreate(RE::Tile* tile) {
               return CALL_MEMBER_FN(tile, GetFloatTraitValue)(CobbPatches::TagIDs::_traitKeepNiProperties) == 2.0F;
            };
            __declspec(naked) void OuterNoCreate() {
               _asm {
                  push esi;
                  call InnerNoCreate;
                  add  esp, 4;
                  test al, al;
                  jnz  lSkip;
                  mov  eax, 0x00401F00; // FormHeap_Allocate
                  call eax;
                  mov  ecx, 0x00590B0F;
                  jmp  ecx;
               lSkip:
                  add  esp, 4;
                  mov  eax, 0x00590B80;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x00590827, (UInt32)&Outer1);
               WriteRelJump(0x0059085B, (UInt32)&Outer2);
               WriteRelJump(0x005908D5, (UInt32)&Outer3);
               WriteRelJump(0x00590909, (UInt32)&Outer4);
               WriteRelJump(0x00590B0A, (UInt32)&OuterNoCreate); // prevent the game from adding a new NiMaterialProperty and the IM alpha property
            };
         }
         namespace Scale {
            void Inner(RE::Tile3D* tile) {
               auto node = tile->renderedNode;
//_MESSAGE("xxnNiScale hook hit.");
               if (!node)
                  return;
               float f = CALL_MEMBER_FN(tile, GetFloatTraitValue)(CobbPatches::TagIDs::_traitNiScale);
//_MESSAGE("xxnNiScale trait on tile is %f", f);
               if (f > 0)
                  node->m_fLocalScale = (std::min)(f, 1.0F);
            };
            __declspec(naked) void Hook1() {
               _asm {
                  push ecx; // preserve
                  push ecx;
                  call Inner;
                  pop  eax;
                  pop  ecx; // restore
                  mov  eax, 0x00588BD0;
                  call eax;
                  mov  eax, 0x0058F3D9;
                  jmp  eax;
               };
            };
            __declspec(naked) void Hook2() {
               _asm {
                  push ecx; // preserve
                  push ecx;
                  call Inner;
                  pop  eax;
                  pop  ecx; // restore
                  mov  eax, 0x00588BD0;
                  call eax;
                  mov  eax, 0x0058F4CB;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x0058F3D4, (UInt32)&Hook1); // patches Tile::HandleChangeFlags -- a branch that runs if locus == 2
               WriteRelJump(0x0058F4C6, (UInt32)&Hook2); // patches Tile::HandleChangeFlags -- a branch that runs if locus != 2
            };
         };

         __declspec(naked) void HandleTraitChangedHook() {
            _asm {
               cmp  esi, CobbPatches::TagIDs::_traitKeepNiProperties; // trait ID
               je   lMarkModified;
               cmp  esi, CobbPatches::TagIDs::_traitNiScale; // trait ID
               je   lMarkModified;
               push 0x00000FE6;      // reproduce patched-over instruction
               mov  eax, 0x0058B4CA;
               jmp  eax;
            lMarkModified:
               mov  ecx, 0x0058B4D5;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x0058B4C5, (UInt32)&HandleTraitChangedHook);
            KeepNiPropertiesInTile3D::Apply();
            Scale::Apply();
         };
      };
      namespace MaskAltTabbingFromMenuEvents {
         __declspec(naked) void Outer() {
            _asm {
               add  esp, 4;   // patched-over instruction
               cmp  edi, 9;   // if (edi == '\t') {
               jne  lExit;    //
               mov  ecx, dword ptr [esi + 0x118];
               test ecx, 1;   //    if (esi->unk118 & 1) {
               jz   lExit;    //
               xor  edi, edi; //       edi = 0;
               //             //    }
               //             // }
            lExit:
               test edi, edi; // patched-over instruction
               mov  ecx, 0x0058352D;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x00583528, (UInt32)&Outer);
         };
      };
      namespace NativeResolution {
         static bool didPatchSafeZoneGetters = false;

         void Update() {
            //
            // We're super lucky that the UI is the only part of Bethesda's code using the constants 
            // 1280.0F, 960.0F, 1280.0, and 960.0...
            //
            auto   iWidth  = *RE::SInt32ScreenWidth;
            auto   iHeight = *RE::SInt32ScreenHeight;
            float  fWidth  = iWidth;
            float  fHeight = iHeight;
            double dWidth  = fWidth;
            double dHeight = fHeight;
            SafeWriteBuf((UInt32) RE::fNormalizedScreenWidthF,    &fWidth,  sizeof(float));
            SafeWriteBuf((UInt32) RE::fNormalizedScreenHeightF,   &fHeight, sizeof(float));
            SafeWriteBuf((UInt32) RE::fNormalizedScreenWidthDbl,  &dWidth,  sizeof(double));
            SafeWriteBuf((UInt32) RE::fNormalizedScreenHeightDbl, &dHeight, sizeof(double));
            _MESSAGE("[PATCH][Miscellaneous::NativeResolution] UI's normalized resolution should now be %dx%d.", iWidth, iHeight);
         };
         __declspec(naked) void HookInterfaceConstructor() {
            __asm {
               call Update;
               mov  edi, dword ptr [esi + 0x78];
               cmp  edi, ebx;
               mov  eax, 0x00580391;
               jmp  eax;
            };
         };
         void Apply() {
            if (!NorthernUI::INI::Display::bForceNativeResolutionUI.bCurrent)
               return;
            WriteRelJump(0x0058038C, (UInt32) &HookInterfaceConstructor);
            _MESSAGE("[PATCH][Miscellaneous::NativeResolution] Applied.");
         };
      };
      namespace ReliableUIShiftModifier {
         //
         // I've observed situations where the game fails to detect when you 
         // release the Shift key, causing it to get "stuck." This code just 
         // adds an extra failsafe to ensure that if the Shift keys aren't 
         // down, the game WILL clear the Shift bit in InterfaceManager's 
         // modifier key state bitmask.
         //
         void Inner(RE::InterfaceManager* ui) {
            auto input = RE::OSInputGlobals::GetInstance();
            if (!CALL_MEMBER_FN(input, QueryKeyboardKeyState)(DIK_LSHIFT, RE::kKeyQuery_Hold))
               if (!CALL_MEMBER_FN(input, QueryKeyboardKeyState)(DIK_RSHIFT, RE::kKeyQuery_Hold))
                  ui->unk118 &= ~4;
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0057F7A0; // reproduce patched-over call
               call eax;             //
               push eax;
               push esi;
               call Inner;
               add  esp, 4;
               pop  eax;
               mov  ecx, 0x0058350C;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x00583507, (UInt32)&Outer);
         };
      };
      namespace TextureFilteringOverride {
         namespace SetFilterMode {
            void Inner(RE::TileImage* tile, RE::NiTexturingProperty* prop) {
               auto map = prop->m_maps.data[0];
               map->mode &= 0xF0FF;
               {  // Disable texture filtering if we're dealing with the cursor.
                  //
                  // For unknown reasons, UI texture filtering is not stable. If the cursor has 
                  // bilinear-interpolation texture filtering, then bilinear-interpolation text-
                  // ure filtering on all other tiles will break: textures will appear blurry, 
                  // and downscaled textures will appear as if they've been pixelated and THEN 
                  // blurred.
                  //
                  // This bug occurs at any moment that the cursor has texture filtering. If we 
                  // toggle its texture filtering at run-time, we also toggle the bug at run-
                  // time.
                  //
                  auto ui = *RE::ptrInterfaceManager;
                  if (ui && ui->cursor == tile || !tile->parent) // This code can run before the cursor's pointer has been saved to the IM. No image tile in an actual interface should lack a parent, though, so check that.
                     return;
               }
               map->mode |= (NorthernUI::INI::Core::uTextureFilteringFlags.iCurrent & 0x0F00); // Else, force the texture-filtering mode we want.
            };
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x004057B0; // replicate patched-over NiTexturingProperty::SetFirstMapTexture call
                  call eax;
                  push esi;
                  push ebp;
                  call Inner;
                  add  esp, 8;
                  mov  eax, 0x00591886;
                  jmp  eax;
               };
            };
         };
         //
         void Apply() {
            if (!NorthernUI::INI::Core::bPatchTextureFiltering.bCurrent)
               return;
            WriteRelJump(0x00591881, (UInt32)&SetFilterMode::Outer);
         };
      };
      namespace KeynavSafeDefaultFix {
         //
         // When you close a menu with keynav, the game needs to redirect keynav focus (i.e. 
         // "mouseover" state) to a safe default tile. The game attempts to pick the first 
         // list item in an applicable list, or the tile with the highest xdefault, from the 
         // topmost menu (by depth-trait).
         //
         // Unfortunately, there's an extremely rare edge case where the game actually fails 
         // to get the right topmost menu. If the topmost menu is invisible, but is queued 
         // to fade back in (as in the case of closing a submenu and returning to its opener), 
         // the menu can be missed -- and the game will pick a "safe" default tile from the 
         // menu beneath it!
         //
         // In my case, this manifested when opening the OptionsMenu from the MainMenu, and 
         // then opening a submenu of the OptionsMenu and closing it with a gamepad button. 
         // Keynav focus returned to the "Options" tile in the MainMenu, rather than the 
         // proper tile in the OptionsMenu, because the OptionsMenu's visible trait was set 
         // to &false;, as the menu was about to start fading back in. This occurred even 
         // when all relevant menus had their menufade and explorefade traits set to zero.
         //
         // We ultimately address this by patching the subroutine that gets the safe default 
         // tile, to use a different function for getting the topmost menu. This function 
         // allows menus that are fading in (unk24 == 8). The patched subroutine still 
         // disqualifies the menu it receives on the grounds that that menu isn't visible, 
         // but it doesn't end up using a flat-out wrong menu instead.
         //
         RE::Menu* Replacement() {
            auto      ui = RE::InterfaceManager::GetInstance();
            auto      root = ui->menuRoot;
            float     depth = -1.0e+08;
            RE::Menu* result = nullptr;
            if (!root)
               return nullptr;
            for (auto node = root->childList.start; node; node = node->next) {
               auto tile = node->data;
               if (!tile)
                  continue;
               auto menu = (RE::Menu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
               if (!menu)
                  continue;
               if (menu->unk24 == 2)
                  continue;
               if (menu->unk24 != 8) { // this is what we changed: we want to allow invisible menus if they're fading in
                  if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_visible) == 1.0F)
                     continue;
               }
               if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_stackingtype) == (float)RE::kEntityID_does_not_stack)
                  continue;
               float current = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_depth);
               if (current > depth) {
                  depth = current;
                  result = menu;
               }
            }
            return result;
         };
         void Apply() {
            WriteRelCall(0x0057DAD2, (UInt32)&Replacement);
         };
      };
      namespace MipMapSkipFix {
         //
         // Oblivion's texture quality option is implemented as iTexMipMapSkip, which does 
         // what it sounds like: it skips the first X mipmaps in every texture. Problem is, 
         // some DDS exporters (particularly GIMP) write a dwMipMapCount of 1 for non-
         // mipmapped textures, when it should be 0. This causes Oblivion to read past the 
         // end of the texture and display garbage data, when it tries to skip mipmaps.
         //
         // This is an issue with the assets, not the game, but I'm going to patch in some 
         // handling for it for two reasons:
         //
         // 1. It'll make things more reliable for anyone creating assets with GIMP.
         //
         // 2. I'm not going to re-save all of my goddamned assets. Screw that.
         //
         struct LoadedDDSFile { // literally just the DDS spec, with the DDS_HEADER values we need
            UInt32     dwMagic;
            DDS_HEADER header;
         };
         UInt32 __stdcall Inner(LoadedDDSFile* loadedTexture, UInt32 fileSize) { // should return number of mipmaps
            auto&  header = loadedTexture->header;
            UInt32 mips   = header.mipMapCount;
            //
            // If the mipmap count is 1 and the texture is compressed, investigate further. It may 
            // be a non-mipmapped texture with a bad count.
            //
            if (mips == 1 && (header.flags & 0x80000) != 0) {
               constexpr int headerSize = 128;
               //
               // For a compressed texture, dwPitchOrLinearSize is the (post-compression) size of the 
               // "top-level" texture (i.e. the main one, and not any mipmaps). If there's nothing 
               // else in the file, then there are no mipmaps.
               //
               if (header.pitchOrLinearSize == fileSize - headerSize)
                  return 0;
            }
            return mips;
         };
         __declspec(naked) void Outer() {
            _asm {
               push edx; // protect
               push edi; // texture file size in bytes
               push ebp; // texture file's mipmap count
               call Inner; // stdcall; no cleanup needed
               pop  edx; // restore
               lea  ecx, [ebp + 0x80];           // reproduce patched-over instruction
               mov  dword ptr [esp + 0x20], ecx; // reproduce patched-over instruction
               mov  ecx, 0x00760FD6;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x00760FC9, (UInt32)&Outer);
            SafeWrite32 (0x00760FCE, 0x90909090); // courtesy NOPs
         };
      };
      namespace PrefabPathSlashFix {
         //
         // Prefabs must specify backslashes when referring to files in BSAs. 
         // Forward slashes trigger missing-file errors.
         //
         void Inner(char* path) {
            while (*path) {
               if (*path == '/')
                  *path = '\\';
               path++;
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               push edi;
               call Inner;
               add  esp, 4;
               push 0x00A3207C; // reproduce patched-over instruction
               mov  eax, 0x0058522B;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x00585226, (UInt32)&Outer);
         };
      };
      namespace ImplementPrioritizedTraitRefs {
         //
         // Make it possible to do things like
         //
         //    <xdown>
         //       <ref name="list_number_one" trait="mouseover" />
         //       <ref name="list_number_two" trait="mouseover" />
         //    </xdown>
         //
         // such that focus is sent to list_number_one if the list has any items in it, 
         // or to list_number_two otherwise. Specifically, we prefer the REF that points 
         // to a tile for which IsValidRefTarget returns true.
         //
         bool IsValidRefTarget(RE::Tile* tile) {
            if (!tile)
               return false;
            if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_target) == 2.0F)
               return true;
            if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_xlist) == 108.0F) { // see code starting at 0x00581203
               for (auto node = tile->childList.end; node; node = node->prev) {
                  auto child = node->data;
                  if (!child)
                     continue;
                  if (!CALL_MEMBER_FN(child, NiNodeIsNotCulled)())
                     continue;
                  if (!CALL_MEMBER_FN(child, GetTrait)(kTileValue_xdefault))
                     continue;
                  return true;
               }
            }
            return false;
         };
         RE::Tile* Inner(RE::Tile::Value* value, UInt32* outTraitID) {
            auto o = value->operators;
            if (!o)
               return nullptr;
            do {
               if (o->opcode == RE::kTagID_ref)
                  break;
            } while (o = o->next);
            if (!o)
               return nullptr;
            auto r = o;
            while (r->refPrev)
               r = r->refPrev;
            RE::Tile::Value* target = r->operand.ref;
            RE::Tile* tile = target->owner;
            //
            // At this point, we pretty much have the result of the vanilla process. Now for 
            // our changes.
            //
            {  // Modded behavior
               RE::Tile*        altTile   = tile;
               RE::Tile::Value* altTarget = nullptr;
               while (!IsValidRefTarget(altTile)) {
                  //
                  // Allow an XML author to specify a prioritized list of REF operators.
                  //
                  o = o->next;
                  if (!o)
                     break;
                  do {
                     if (o->opcode == RE::kTagID_ref)
                        break;
                  } while (o = o->next);
                  if (!o)
                     break;
                  auto r = o;
                  while (r->refPrev)
                     r = r->refPrev;
                  altTarget = r->operand.ref; // we can never actually guarantee this IS a pointer and not an immediate, and that makes me nervous, hence the weird check below
                  altTile   = altTarget ? altTarget->owner : nullptr;
               }
               if (o && altTile && (UInt32)altTarget > 0x400000) { // it's not a real pointer if it's below Oblivion.exe's module base, and yes, this IS a filthy hack
                  tile   = altTile;
                  target = altTarget;
               }
               //
               // NOTE: It's perfectly normal not to find a "valid ref target." The function we're 
               // patching is called multiple times while processing keyboard navigation, and will 
               // sometimes return non-targetable tiles such as list containers. We don't want to 
               // simply fail out with nullptr if a non-targetable tile is the normal result; we 
               // only want to let an author specify multiple REF operators and prefer the one that 
               // returns a suitable tile.
            }
            *outTraitID = target->id;
            return tile;
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, dword ptr [esp + 0x28];
               push eax;
               push ecx;
               call Inner;
               add  esp, 8;
               //
               // mimic vanilla return:
               //
               pop  edi;
               pop  esi;
               pop  ebp;
               pop  ebx;
               add  esp, 0x10;
               retn 8;
            };
         };
         void Apply() {
            WriteRelJump(0x0058E772, (UInt32)&Outer); // patch Tile::ResolveTraitReference
         };
      };
      namespace SuppressQuantityMenu {
         //
         // Reproduce the functions of Toggleable Quantity Prompts, but at the native 
         // code level.
         //
         SInt32 CheckBarterConfirmPref() {
            auto ui = RE::InterfaceManager::GetInstance();
            if (ui->unk118 & 1)
               return NorthernUI::INI::Features::iBarterConfirmHandlerAlt.iCurrent;
            if (ui->unk118 & 2)
               return NorthernUI::INI::Features::iBarterConfirmHandlerCtrl.iCurrent;
            return NorthernUI::INI::Features::iBarterConfirmHandlerDefault.iCurrent;
         };
         SInt32 CheckQuantityMenuPref() {
            auto ui = RE::InterfaceManager::GetInstance();
            if (ui->unk118 & 1)
               return NorthernUI::INI::Features::iQuantityMenuHandlerAlt.iCurrent;
            if (ui->unk118 & 2)
               return NorthernUI::INI::Features::iQuantityMenuHandlerCtrl.iCurrent;
            return NorthernUI::INI::Features::iQuantityMenuHandlerDefault.iCurrent;
         };
         //
         namespace SuppressBarterConfirmation {
            //
            // Toggleable Quantity Prompts also lets you auto-confirm the barter prompt.
            //
            // ContainerMenu's code is horribly messy, at least when compiled, so our patch 
            // is also messy.
            //
            //  - ContainerMenu REFUSES to transfer an item if a message box doesn't appear, 
            //    even though there's nothing in the message box callback that would suggest 
            //    why. As such, instead of running the callback directly, we have to actually 
            //    pop the message box and automatically dismiss it on the next frame.
            //
            //  - HandleMouseUp runs before HandleFrame within a frame, so we need a counter 
            //    so that HandleFrame can distinguish between the current and next frame.
            //
            //  - If the QuantityMenu pops up, we have to wait for that to go away before we 
            //    start counting frames for MessageMenu.
            //
            //  - The price is only retrieved when building the confirmation prompt message 
            //    text or when carrying out the transaction, so we need to swipe it during 
            //    the former step. (We could just call the getter again, but the values 
            //    supplied to it are either not readily available when we need them, or not 
            //    available at all.)
            //
            static float s_lastPrice = 0.123456F; // obviously wrong value, used for logging purposes while debugging
            //
            namespace InterceptPrice {
               //
               // The price is only computed when formatting the confirmation prompt text, 
               // and isn't retained otherwise. There's no easy way to reproduce the calls 
               // involved, so we just patch after them and cache the price in a static 
               // variable.
               //
               void _stdcall Inner(float p) {
                  s_lastPrice = p;
               };
               __declspec(naked) void OuterSale() {
                  _asm {
                     push eax; // protect
                     mov  eax, dword ptr [esp + 0x1C]; // == price * quantity // esp18 is the price; we pushed onto the stack first, so 18+4==1C
                     push eax;   //
                     call Inner; // stdcall
                     pop  eax; // restore
                     fstp st(1);    // reproduce patched-over instruction
                     test ah, 0x05; // reproduce patched-over instruction
                     mov  ecx, 0x0059A702;
                     jmp  ecx;
                  };
               };
               __declspec(naked) void OuterBuy() {
                  _asm {
                     mov  eax, dword ptr [esp + 0x18]; // == price * quantity
                     push eax;   //
                     call Inner; // stdcall
                     cmp  edi, 1; // reproduce patched-over instruction
                     fld  dword ptr [esp + 0x18]; // reproduce patched-over instruction
                     mov  edx, 0x0059A85C;
                     jmp  edx;
                  };
               };
               void Apply() {
                  WriteRelJump(0x0059A6FD, (UInt32)&OuterSale); // ContainerMenu::HandleMouseUp // TODO: BROKEN
                  WriteRelJump(0x0059A855, (UInt32)&OuterBuy);  // ContainerMenu::HandleMouseUp
                  SafeWrite16 (0x0059A85A, 0x9090); // courtesy NOP
               };
            };
            //
            bool ShouldShowConfirmationPrompt() {
               switch (CheckBarterConfirmPref()) {
                  case NorthernUI::kBarterConfirmHandler_Always:
                     return true;
                  case NorthernUI::kBarterConfirmHandler_IfNotFree:
                     if (s_lastPrice > 0.001F) // float precision is terrible; Bethesda's price math can yield numbers like -3e-40 and such
                        return true;
                     return false;
                  case NorthernUI::kBarterConfirmHandler_Never:
                     return false;
               }
               return true;
            };
            void AutoAnswerMessage() {
               auto tile = (RE::Menu*) GetMenuByType(RE::kMenuID_MessageMenu);
               if (tile)
                  tile->HandleMouseUp(4, nullptr); // MessageMenu buttons start at ID #4; we want to click the 0th button
            };

            enum {
               kFrameState_NotReady  = 0,
               kFrameState_SameFrame = 1,
               kFrameState_NextFrame = 2,
            };
            static UInt32 s_frame = kFrameState_NotReady;
            //
            void Inner_Frame() {
               if (s_frame == kFrameState_SameFrame)
                  s_frame = kFrameState_NextFrame;
               else if (s_frame == kFrameState_NextFrame) {
                  AutoAnswerMessage();
                  s_frame = kFrameState_NotReady;
               }
            };
            __declspec(naked) void Outer_Frame() {
               static_assert(offsetof(RE::ContainerMenu, quantityMenuInProgress) == 0x54, "ContainerMenu's flag for the QuantityMenu being open has moved! Update this x86 patch!");
               _asm {
                  push eax; // protect
                  mov  al, byte ptr [esi + 0x54]; // bool al = this->unk54;
                  test al, al;
                  jnz  lAfter; // if the QuantityMenu is open, then defer until it isn't, so we can catch the MessageMenu (yes, I know this is ugly)
                  call Inner_Frame;
               lAfter:
                  pop  eax; // restore
                  mov  ecx, dword ptr [eax + 0x20]; // reproduce patched-over subroutine
                  push 5; // reproduce patched-over subroutine
                  mov  eax, 0x0059871F;
                  jmp  eax;
               };
            };
            void PrepToAutoAnswer() {
               s_frame = kFrameState_SameFrame;
            };
            __declspec(naked) void Outer_MouseUp() {
               //
               // Patches right over a call to ShowMessageBox after its args have been pushed
               //
               _asm {
                  call ShouldShowConfirmationPrompt;
                  test al, al;
                  jnz  lShow;
                  call PrepToAutoAnswer;
               lShow:
                  mov  eax, 0x00579C10; // ShowMessageBox
                  call eax; // reproduce patched-over call
               lExit:
                  mov  eax, 0x0059A90A; // jump to the ADD ESP, 0x18 instruction that cleans up ShowMessageBox's args
                  jmp  eax;
               };
            };
            void Apply() {
               InterceptPrice::Apply();
               WriteRelJump(0x0059A905, (UInt32)&Outer_MouseUp); // ContainerMenu::HandleMouseUp
               WriteRelJump(0x0059871A, (UInt32)&Outer_Frame);   // ContainerMenu::HandleFrame
            };
         };
         //
         UInt32 __stdcall InnerContainer(SInt32 count) { // returns address to jump back to
            switch (CheckQuantityMenuPref()) {
               case NorthernUI::kQuantityHandler_TakeAll:
                  *(SInt32*)(0x00B13E94) = count; // *g_ContainerMenu_Quantity = eax;
               case NorthernUI::kQuantityHandler_TakeOne:
                  return 0x0059A688; // handles taking an item; if g_ContainerMenu_Quantity isn't -1, then takes that many of the item
            }
            return 0x0059A785; // pops QuantityMenu if necessary
         };
         __declspec(naked) void OuterContainer() {
            _asm {
               mov  eax, dword ptr [ecx + 0x4];
               push eax; // protect
               push eax;
               call InnerContainer; // stdcall
               mov  ecx, eax;
               pop  eax; // restore
               jmp  ecx;
            };
         };
         //
         UInt32 InnerInventory(RE::ExtraContainerChanges::EntryData* item) { // returns address to jump back to
            switch (CheckQuantityMenuPref()) {
               case NorthernUI::kQuantityHandler_TakeAll:
                  *(SInt32*)(0x00B140E4) = item->countDelta;
                  return 0x005ABF4B; // handles dropping an item if g_ContainerMenu_Quantity isn't -1
               case NorthernUI::kQuantityHandler_TakeOne:
                  *(SInt32*)(0x00B140E4) = 1;
                  return 0x005ABF4B; // handles dropping an item if g_ContainerMenu_Quantity isn't -1
            }
            return 0x005ABE80; // pops QuantityMenu if necessary
         };
         __declspec(naked) void OuterInventory() {
            //
            // Hook for dropping items.
            //
            _asm {
               mov  eax, 0x00B140E4;
               cmp  dword ptr [eax], -1; // *g_InventoryMenu_Quantity == -1?
               jne  lExistingQuantity;
               push esi;
               call InnerInventory;
               add  esp, 4;
               jmp  eax;
            lExistingQuantity:
               mov  eax, 0x005ABF4B;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x0059A780, (UInt32)&OuterContainer); // ContainerMenu::HandleMouseUp
            //
            WriteRelJump(0x005ABE73, (UInt32)&OuterInventory);
            SafeWrite16 (0x005ABE78, 0x9090); // courtesy NOP
            //
            SuppressBarterConfirmation::Apply();
         };
      };
      namespace ZoomTraitUpdatesChangeFlags {
         void _stdcall Inner(RE::Tile* tile) {
            tile->flags |= RE::Tile::kTileFlag_ChangedImage; // forcibly redraw image
            tile->flags |= RE::Tile::kTileFlag_ChangedSize;
         };
         __declspec(naked) void Outer() {
            _asm {
               //
               // Minor note: the float value that the trait has changed to is available 
               // here as [esp + 0x14].
               //
               cmp  eax, 0x387; // reproduce patched-over branch
               je   lIsText;
               cmp  eax, 0x386; // image
               jne  lDone;
               cmp  esi, 0xFD2; // RE::kTagID_zoom
               jne  lDone;
               //or dword ptr [edi + 0x2C], 0x20; // RE::Tile::kTileFlag_ChangedImage
               push edi;
               call Inner; // stdcall
            lDone:
               mov  eax, 0x0058B36F;
               jmp  eax;
            lIsText:
               mov  eax, 0x0058B333;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x0058B32C, (UInt32)&Outer); // patch Tile::HandleTraitChanged
         };
      };
      namespace FixReloadHUDCrash { // experimental
         //
         // Using the "reload" console command to reload any HUD menus causes an instant 
         // crash. My crash logger tracks it, oddly enough, to code deep inside of the 
         // ModelLoader -- specifically,
         //
         // LockFreeCaseInsensitiveStringMap<Model*>::Unk_01(const char* arg1)
         //
         // If arg1 is nullptr -- and apparently, it is in this case -- then we crash. 
         // What the subroutine is supposed to do is convert it to lowercase and store 
         // it on the stack.
         //
         // Testing indicates no obvious consequences to just... skipping the loop that 
         // does the string conversion. Huh. Well, okay.
         //
         void _stdcall DebugLog(UInt32 caller) {
            _MESSAGE("WARNING: LockFreeCaseInsensitiveStringMap<Model*>::Unk_01(nullptr) ! Caller appears to be %08X.", caller);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  ebx, ecx; // reproduce skipped instruction
               push edi; // reproduce patched-over instruction, albeit out of order
               lea  edi, [esp + 0x10]; // reproduce skipped instruction
               test esi, esi;
               jz   lExit;
               mov  al, byte ptr [esi]; // reproduce patched-over instruction, albeit out of order
               test al, al; // reproduce patched-over instruction, albeit out of order
               mov  ecx, 0x0043E6B2;
               jmp  ecx;
            lExit:
               //
               mov  eax, esp;
               add  eax, 0x3FC;
               push eax;
               call DebugLog; // stdcall
               //
               xor  eax, eax;
               mov  ecx, 0x0043E6CC;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x0043E6A5, (UInt32)&Outer);
         };
      };

      void Apply() {
         //
         // Each miscellaneous patch is responsible for testing its own INI settings.
         //
         NativeResolution::Apply();
         TextureFilteringOverride::Apply();
         Tile3DRenderChanges::Apply();
         MaskAltTabbingFromMenuEvents::Apply();
         ReliableUIShiftModifier::Apply();
         KeynavSafeDefaultFix::Apply();
         MipMapSkipFix::Apply();
         PrefabPathSlashFix::Apply();
         ImplementPrioritizedTraitRefs::Apply();
         SuppressQuantityMenu::Apply();
         ZoomTraitUpdatesChangeFlags::Apply();
         FixReloadHUDCrash::Apply();
      };
   };
};