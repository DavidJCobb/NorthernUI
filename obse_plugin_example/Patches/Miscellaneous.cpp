#include "Miscellaneous.h"
#include "obse_common/SafeWrite.h"
#include "Services/INISettings.h"
#include "Patches/TagIDs/Main.h"
#include "ReverseEngineered/NetImmerse/NiObject.h"
#include "ReverseEngineered/NetImmerse/NiProperty.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/Script.h"

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
            void Apply() {
               WriteRelJump(0x00590827, (UInt32)&Outer1);
               WriteRelJump(0x0059085B, (UInt32)&Outer2);
               WriteRelJump(0x005908D5, (UInt32)&Outer3);
               WriteRelJump(0x00590909, (UInt32)&Outer4);
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
               //
               // For a compressed texture, dwPitchOrLinearSize is the (post-compression) size of the 
               // "top-level" texture (i.e. the main one, and not any mipmaps). If there's nothing 
               // else in the file, then there are no mipmaps.
               //
               if (header.pitchOrLinearSize == fileSize - 128)
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
      };
   };
};