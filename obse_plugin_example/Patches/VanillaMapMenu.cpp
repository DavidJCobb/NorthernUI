#include "VanillaMapMenu.h"
#include "obse_common/SafeWrite.h"

#include "XboxGamepad/Main.h"
#include "XboxGamepad/CustomControls.h"
#include "XboxGamepad/Patch.h"

#include "Miscellaneous/strings.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/MapMenu.h"
#include "obse/NiObjects.h"
#include "obse/GameTiles.h"
#include "Services/INISettings.h"

#include "obse/NiGeometry.h" // for DebugHook

namespace CobbPatches {
   namespace MapMenu {
      /*namespace AllowChangingShowOnMapTile {
         //
         // Patch: I have some concerns about whether MapMenu will properly handle the visibility 
         // of the Show On Map button if its ID changes at run-time (i.e. if two tiles "trade" the 
         // title of Glorious Show On Map Button, Esq.). If we do see any bugs with that, this 
         // patch should address them. You'll need to define that subroutine on PlayerCharacter 
         // and include the relevant headers for g_thePlayer in order to use the patch.
         //
         // EDIT: This never turned out to be needed.
         //
         void Inner(RE::Tile* tile) {
            UInt32 a = CALL_MEMBER_FN(*g_thePlayer, Subroutine0065D830)();
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_visible, a ? 2.0F : 1.0F);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  dword ptr [ecx + 0x50], edx;
               push edx;
               call Inner;
               add  esp, 4;
               retn 8;
            };
         };
         void Apply() {
            WriteRelJump(0x005B66F6, (UInt32)&Outer);
            SafeWrite8 (0x005B66FB, 0x90);
         };
      };*/
      namespace LocalMapHooks {
         namespace FixSettingWaypoint {
            //
            // Setting a custom marker on the local map can cause the game to call 
            // UpdateMapMenuLocalMap, which deletes and regenerates all icons on 
            // the local map. If the player currently has a tooltip on an icon, 
            // the "current tooltip-targeted icon" pointer isn't wiped, so it can 
            // end up pointing to a destroyed tile.
            //
            // I think.
            //
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x00573880; // NiTList::DestroyAllNodes
                  call eax;             // reproduce patched-over call
                  //
                  // At this point, if edi->hoveredMapIcon is not null, then it is a 
                  // bad pointer. We can't call edi->ForceDismissTooltip to clear it, 
                  // because that tries to change traits on the tile if the pointer 
                  // isn't null. We need to null the pointer and *then* call that 
                  // subroutine.
                  //
                  // (The reason we need to dismiss the tooltip at all is to account 
                  // for the case where the mouse passes over an icon, and then the 
                  // gamepad is used to set a custom marker elsewhere. If we fail to 
                  // dismiss the tooltip, then it gets "stuck" until it's "needed 
                  // elsewhere."
                  //
                  // (As for nulling the pointer? Self-explanatory. If we don't null 
                  // it, then the next thing that tries to use it will crash. I guess 
                  // Bethesda just missed it, and one of our other patches just made 
                  // the crash more likely by virtue of using that pointer.)
                  //
                  mov  dword ptr [edi + 0xF4], 0; // edi->hoveredMapIcon = nullptr;
                  mov  ecx, edi;
                  call RE::MapMenu::ForceDismissTooltip; // edi->ForceDismissTooltip();
                  mov  eax, 0x005BA5E3;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005BA5DE, (UInt32)&Outer);
            };
            static_assert(offsetof(RE::MapMenu, hoveredMapIcon) == 0xF4, "The offset of RE::MapMenu::hoveredMapIcon has changed from 0xF4. Update the inline assembly code bundled with this assert to use the new offset.");
         };
         namespace UseBackground {
            static bool doRenderBackground = false; // Needed so the cell render can tell what we specified in XML

            bool Inner(RE::MapMenu* menu) {
               //_MESSAGE("MapMenu background bool hook tripped. Current MapMenu is at %08X.", menu);
               //DebugLogs(menu);
               if (menu) {
                  float value;
                  auto  tile = menu->localMap;
                  if (tile && tile->GetFloatValue(kTileValue_user24, &value)) {
                     return (doRenderBackground = (value != 1.0F));
                  }
               }
               doRenderBackground = true;
               return true;
            };
            __declspec(naked) void OuterMapRender() {
               __asm {
                  mov  eax, dword ptr [esp + 0x88];
                  //push ecx; // a review of the vanilla code suggests this register is disposable here
                  //push edx; // a review of the vanilla code suggests this register is disposable here
                  push eax;    //
                  call Inner;  //
                  add  esp, 4; // Inner(*(esp + 0x88));
                  //pop  edx;
                  //pop  ecx;
                  mov  byte ptr [esp + 0x1B], al;
                  //
                  // Though we didn't patch over the {CMP DWORD PTR [0x00B42F48], 2} instruction, we will almost certainly have 
                  // blown its flags away by now. We need to re-execute it (but written slightly differently because MASM is 
                  // stupid sometimes).
                  //
                  mov  eax, 0x00B42F48;
                  cmp  dword ptr [eax], 2;
                  //
                  mov  eax, 0x005B76C2;
                  jmp  eax;
               };
            };
            __declspec(naked) void OuterCellRender() {
               __asm {
                  mov  bl, doRenderBackground;
                  mov  byte ptr [esp + 0x14], bl;
                  mov  eax, 0x004D01F9;
                  jmp  eax;
               };
            };
            //
            void Apply() {
               WriteRelJump(0x005B76BD, (UInt32) &OuterMapRender);
               //
               WriteRelJump(0x004D01E8, (UInt32) &OuterCellRender);
               SafeWrite8(0x004D01ED, 0x90); // NOP
            };
         };
         namespace OverridePath {
            UInt32 Inner(RE::MapMenu* menu) {
               //_MESSAGE("MapMenu background path hook tripped. Current MapMenu is at %08X.", menu);
               if (menu) {
                  const char* value;
                  auto tile = menu->localMap;
                  if (tile && tile->GetStringValue(kTileValue_user25, &value) && cobb::string_has_content(value))
                     return (UInt32) value;
               }
               return 0x00A4D088; // address of the default path
            };
            __declspec(naked) void Outer() {
               __asm {
                  //
                  // Expendable registers: eax, ebp, ecx, esi
                  //
                  // We are patching over a PUSH opcode, which is used to pass a const char* as an argument. 
                  // As such, we need to PUSH the const char* filename that we want to use and then jump back 
                  // to the original subroutine.
                  //
                  mov  eax, dword ptr [esp + 0x90]; // *(esp + 0x88) == this; we have pushed two arguments already, so 0x90
                  push edx;                         // Compilers consider EDX a temporary register; we need to protect it in case something we call uses it.
                  push eax;    // //
                  call Inner;  // // Inner(*(esp + 0x88);
                  add  esp, 4; // // Clean up after non-member call.
                  pop  edx;                         // Restore EDX before returning.
                  push eax;
                  mov  eax, 0x005B76F2;
                  jmp  eax;
               };
            };
            //
            void Apply() {
               WriteRelJump(0x005B76ED, (UInt32) &Outer);
            };
         };
         namespace OverrideRenderResolution {
            void OnINIChange(NorthernUI::INI::INISetting* s, NorthernUI::INI::ChangeCallbackArg oldVal, NorthernUI::INI::ChangeCallbackArg newVal) {
               auto& desired = NorthernUI::INI::Display::uLocalMapResolutionPerCell;
               if (!s || s == &desired) {
                  auto resolution = desired.iCurrent;
                  if (!resolution)
                     resolution = 256;
                  SafeWrite32(0x004D0225, resolution);
                  SafeWrite32(0x004D0260, resolution);
               }
            };
            void Apply() {
               UInt32 resolution = NorthernUI::INI::Display::uLocalMapResolutionPerCell.iCurrent;
               if (!resolution)
                  resolution = 256; // vanilla default
               //
               // Overwrite arguments passed to vanilla method calls deep within a TESObjectCELL method: 
               // change the local map render resolution (per cell) from 256px to INI-specified value.
               //
               SafeWrite32(0x004D0225, resolution);
               SafeWrite32(0x004D0260, resolution);
               //
               NorthernUI::INI::RegisterForChanges(&OnINIChange);
            };
         };
         //
         void Apply() {
            FixSettingWaypoint::Apply(); // Patch: Fix potential CTDs in connection to setting custom waypoints on the local map
            UseBackground::Apply(); // Patch: Allow XML to override whether to render a background texture
            OverridePath::Apply();  // Patch: Allow XML to override the background texture by specifying user25
            OverrideRenderResolution::Apply(); // Patch: increase render resolution for the local map
         };
      };
      namespace WorldMapHooks {
         //
         // See also: the DynamicMapEmulation patch
         //
         namespace SafeHoveredMapIcon {
            //
            // Ensure the hoveredMapIcon isn't deleted out from the menu. Speculative.
            //
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x00573880; // NiTList::DestroyAllNodes
                  call eax;             // reproduce patched-over call
                  //
                  // At this point, if edi->hoveredMapIcon is not null, then it is a 
                  // bad pointer. We can't call edi->ForceDismissTooltip to clear it, 
                  // because that tries to change traits on the tile if the pointer 
                  // isn't null. We need to null the pointer and *then* call that 
                  // subroutine.
                  //
                  // (The reason we need to dismiss the tooltip at all is to account 
                  // for the case where the mouse passes over an icon, and then the 
                  // gamepad is used to set a custom marker elsewhere. If we fail to 
                  // dismiss the tooltip, then it gets "stuck" until it's "needed 
                  // elsewhere."
                  //
                  // (As for nulling the pointer? Self-explanatory. If we don't null 
                  // it, then the next thing that tries to use it will crash. I guess 
                  // Bethesda just missed it, and one of our other patches just made 
                  // the crash more likely by virtue of using that pointer.)
                  //
                  mov  dword ptr [esi + 0xF4], 0; // edi->hoveredMapIcon = nullptr;
                  mov  ecx, esi;
                  call RE::MapMenu::ForceDismissTooltip; // edi->ForceDismissTooltip();
                  mov  eax, 0x005B9648;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005B9643, (UInt32)&Outer);
            };
            static_assert(offsetof(RE::MapMenu, hoveredMapIcon) == 0xF4, "The offset of RE::MapMenu::hoveredMapIcon has changed from 0xF4. Update the inline assembly code bundled with this assert to use the new offset.");
         };
      };
      namespace QuestListHooks {
         namespace AccuratePaneHeight {
            //
            // Patch: Supply the quest list pane with the end Y-coordinate of the last generated 
            // tile in the quest/objective list.
            //
            // Vanilla code sets the quest list pane's height to the total height of its contents; 
            // this is needed to allow for proper scrollbar behavior. However, if you want to add 
            // spacing between list items, that breaks down. This patch sets the quest list pane's 
            // USER20 trait to the Y-coordinate of the last generated list item plus the height of 
            // that list item.
            //
            // There's a hurdle, though. We can't just do that when the list is generated, because 
            // list items might vary their heights based on external factors. NorthernUI, for exam-
            // ple, has list items vary in height based on visibility of the "Back" button. (If 
            // the "Back" button is visible, then the list items are quest objectives, not quests. 
            // In that case, we only have the first one show an icon and quest name.) The "Back" 
            // button's visibility is not always updated before the list items are generated.
            //
            // The easiest way to avoid edge-cases like these is to just patch the frame handler to 
            // blindly update the list pane every frame.
            //
            void Inner(RE::MapMenu* menu) {
               auto pane = menu->journalPane;
               if (!pane)
                  return;
               float end = 0.0F;
               {
                  auto node = pane->childList.start;
                  if (node) {
                     auto tile = node->data;
                     if (tile) {
                        end = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_y);
                        end += CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_height);
                     }
                  }
               }
               CALL_MEMBER_FN(pane, UpdateFloat)(kTileValue_user20, end);
            };
            __declspec(naked) void Outer() {
               _asm {
                  push esi;
                  call Inner;
                  add  esp, 4;
                  cmp  byte ptr [esi + 0x84], 0; // reproduce patched-over instruction
                  mov  eax, 0x005B7119;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005B7112, (UInt32)&Outer);
               SafeWrite16 (0x005B7117, 0x9090); // courtesy NOPs
            };
         };
         namespace SelectionBehavior {
            void Inner(RE::MapMenu* menu, void* originalArg) {
               bool vanilla = true;
               auto root    = (RE::Tile*) menu->tile;
               if (root) {
                  float value;
                  if (root->GetFloatValue(kTileValue_user20, &value) && value == 1.0)
                     vanilla = false;
               }
               if (vanilla)
                  CALL_MEMBER_FN(menu, SetMenuTabIndex)(3, originalArg);
            };
            __declspec(naked) void Outer() {
               __asm {
                  mov  [esp], esi; // replace a pushed argument
                  call Inner;      // use the two already-pushed vanilla args
                  add  esp, 8;     // clean up after last non-member call
                  mov  eax, 0x005BC166;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005BBB27, (UInt32) &Outer);
            };
         };
         //
         void Apply() {
            AccuratePaneHeight::Apply();
            SelectionBehavior::Apply(); // Patch: Allow XML to decide whether selecting a quest punts the user to the Active Quest tab
         };
      };
      namespace XInput {
         RE::Tile* _GetCursorTargetedTile(RE::Tile* canvas, RE::Tile* cursor, float* outX = nullptr, float* outY = nullptr, float* outCanvasDepth = nullptr) {
            float canvasAbsX, canvasAbsY, canvasAbsDepth;
            float cursorAbsX, cursorAbsY, throwaway;
            canvas->GetAbsoluteCoordinates(canvasAbsX, canvasAbsY, canvasAbsDepth);
            cursor->GetAbsoluteCoordinates(cursorAbsX, cursorAbsY, throwaway);
            if (CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_locus) != 2.0F) {
               canvasAbsX -= CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_x);
               canvasAbsY -= CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_y);
               canvasAbsDepth -= CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_depth);
            }
            if (outCanvasDepth)
               *outCanvasDepth = canvasAbsDepth;
            //
            for (auto node = canvas->childList.start; node; node = node->next) {
               auto tile = node->data;
               if (!tile)
                  continue;
               SInt32 id = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_id);
               if (id != RE::MapMenu::kTileID_WorldMapIcon && id != RE::MapMenu::kTileID_LocalMapIcon)
                  continue;
               if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_target) != 2.0F)
                  continue;
               //
               // Bounds-checking:
               //
               float x = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_x) + canvasAbsX;
               if (x > cursorAbsX)
                  continue;
               float width = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_width);
               if (x + width < cursorAbsX)
                  continue;
               float y = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_y) + canvasAbsY;
               if (y > cursorAbsY)
                  continue;
               float height = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_height);
               if (y + height < cursorAbsY)
                  continue;
               if (outX)
                  *outX = x;
               if (outY)
                  *outY = y;
               return tile;
            }
            return nullptr;
         };

         namespace FrameHandler {
            //
            // Handle panning the map using the joysticks, and hovering over and off of map icons 
            // using the joystick-accessible cursor.
            //
            static RE::Tile* s_lastGamepadHover = nullptr; // goal was to avoid interfering with mouse hover (i.e. hovering with one device stops hover from the other), but it doesn't work. gamepad always takes priority
            void Inner(RE::MapMenu* mapMenu) {
               float user21 = CALL_MEMBER_FN((RE::Tile*)mapMenu->tile, GetFloatTraitValue)(kTileValue_user21);
               if (user21 != 2.0F && user21 != 3.0F)
                  return;
               UInt8 page = CALL_MEMBER_FN(mapMenu->background, GetFloatTraitValue)(kTileValue_user0);
               if (page > 2)
                  return;
               RE::Tile* cursor = (page == 2) ? mapMenu->worldCursor : mapMenu->localCursor;
               if (!cursor)
                  return;
               if (CALL_MEMBER_FN(RE::InterfaceManager::GetInstance(), GetTopmostMenuID)() != 1) // return if "Big Four" don't have focus, as that means MapMenu doesn't have focus
                  return;
               SInt32 user22 = CALL_MEMBER_FN((RE::Tile*)mapMenu->tile, GetFloatTraitValue)(kTileValue_user22);
               //
               XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
               if (!gamepad)
                  return;
               SInt32 x, y;
               {
                  auto& manager = XXNGamepadConfigManager::GetInstance();
                  //
                  // No-Swap & No-Three -> Left  | 0 : 0 = 0
                  // No-Swap & Three    -> Right | 0 : 1 = 1
                  // Swap    & No-Three -> Right | 1 : 0 = 1
                  // Swap    & Three    -> Left  | 1 : 1 = 0
                  //
                  bool useRightStick = manager.swapSticksMenuMode != (user21 == 3.0F);
                  auto joyX = useRightStick ? XXNGamepad::kJoystickAxis_RightX : XXNGamepad::kJoystickAxis_LeftX;
                  auto joyY = useRightStick ? XXNGamepad::kJoystickAxis_RightY : XXNGamepad::kJoystickAxis_LeftY;
                  x = gamepad->GetJoystickAxis(joyX);
                  y = gamepad->GetJoystickAxis(joyY);
               }
               float xPan = NorthernUI::INI::XInput::fMapMenuPanSpeed.fCurrent * (float)x / 100.0F;
               float yPan = NorthernUI::INI::XInput::fMapMenuPanSpeed.fCurrent * (float)y / 100.0F;
               //
               float currentX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user0);
               float currentY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user1);
               float minX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user2);
               float minY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user3);
               float maxX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user4);
               float maxY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user5);
               //
               bool  isPanning;
               float xMod = 0.0,
                     yMod = 0.0;
               {
                  if (((user22 & 1) != 0) && gamepad->GetButtonState(XXNGamepad::kGamepadButton_LB, RE::kKeyQuery_Hold)) { 
                     //
                     // USER22: bit 1: force panning if LB is held
                     //
                     isPanning = true;
                     xMod = xPan;
                     yMod = yPan;
                  } else {
                     currentX += xPan;
                     currentY += yPan;
                     bool isPanning = currentX > maxX || currentX < minX || currentY > maxY || currentY < minY;
                     if (isPanning) { // There are single-line ways to write these using std::min and std::max, but this is clearer:
                        xMod = currentX - maxX;
                        if (xMod <= 0.0) {
                           xMod = currentX - minX;
                           if (xMod > 0.0)
                              xMod = 0.0;
                        }
                        yMod = currentY - maxY;
                        if (yMod <= 0.0) {
                           yMod = currentY - minY;
                           if (yMod > 0.0)
                              yMod = 0.0;
                        }
                     }
                  }
                  if (page == 1) { // I'm not even gonna pretend to understand why this is inconsistent between the two maps.
                     xMod = -xMod;
                     yMod = -yMod;
                  }
               }
               currentX = (std::min)((std::max)(currentX, minX), maxX);
               currentY = (std::min)((std::max)(currentY, minY), maxY);
               //
               CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_user0, currentX);
               CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_user1, currentY);
               //
               if (isPanning) {
                  //
                  // The player is moving the cursor past the edge of the visible map. We've 
                  // constrained the cursor to the visible area, but we still need to pan the 
                  // map as the player wishes.
                  //
                  CALL_MEMBER_FN(mapMenu, PanMap)(xMod, yMod, page == 2);
               }
               //
               {  // Handle hovering the Xbox cursor over a map icon:
                  float x, y, canvasAbsDepth = 0;
                  RE::Tile* canvas = (page == 2) ? mapMenu->worldMap : mapMenu->localIcons;
                  RE::Tile* found  = _GetCursorTargetedTile(canvas, cursor, &x, &y, &canvasAbsDepth);
                  //
                  auto tooltip = mapMenu->worldIconPaper;
                  auto previous = mapMenu->hoveredMapIcon;
                  if (found) {
                     if (previous)
                        mapMenu->ForceDismissTooltip();
                     //
                     // This code is based on the content of MapMenu::HandleMouseover:
                     //
                     CALL_MEMBER_FN(found, UpdateFloat)(kTileValue_user7, 100.0F);
                     CALL_MEMBER_FN(tooltip, UpdateFloat)(kTileValue_user0, CALL_MEMBER_FN(found, GetFloatTraitValue)(kTileValue_depth) + canvasAbsDepth);
                     CALL_MEMBER_FN(tooltip, UpdateFloat)(kTileValue_user1, x);
                     CALL_MEMBER_FN(tooltip, UpdateFloat)(kTileValue_user2, y);
                     CALL_MEMBER_FN(tooltip, UpdateString)(kTileValue_string, CALL_MEMBER_FN(found, GetStringTraitValue)(kTileValue_user4));
                     CALL_MEMBER_FN(tooltip, UpdateFloat)(kTileValue_visible, 2.0F);
                     CALL_MEMBER_FN(tooltip, UpdateFloat)(kTileValue_user6, CALL_MEMBER_FN(found, GetFloatTraitValue)(kTileValue_user6));
                     CALL_MEMBER_FN(tooltip, Subroutine0058FBA0)(0);
                     mapMenu->hoveredMapIcon = found;
                     s_lastGamepadHover = found;
                  } else {
                     if (previous && s_lastGamepadHover == previous) {
                        mapMenu->ForceDismissTooltip();
                        s_lastGamepadHover = nullptr;
                     }
                  }
               }
            };
            __declspec(naked) void Outer() {
               _asm {
                  push esi;
                  call Inner;
                  add  esp, 4;
                  pop  esi; // overwritten opcode
                  retn;     // overwritten opcode
               };
            };
            void Apply() {
               WriteRelJump(0x005B7148, (UInt32)&Outer);
            };
         };
         namespace MouseoverHook {
            //
            // The vanilla MapMenu::HandleMouseover was written on the assumption that if 
            // you move the mouse from one map icon to another, there would always be an 
            // intervening MapMenu::HandleMouseout call to clear the map icons' hover state. 
            // However, that's no longer a reliable assumption, because with the gamepad 
            // support you have two cursors that are active concurrently.
            //
            void Inner(RE::MapMenu* mapMenu) {
               auto tile = mapMenu->hoveredMapIcon;
               if (tile) {
//_MESSAGE("DEBUG DEBUG DEBUG - MAPMENU XINPUT MOUSEOVER PATCH - MENU IS %08X WITH VTBL %08X; TILE IS %08X WITH VTBL %08X", mapMenu, *(UInt32*)mapMenu, tile, *(UInt32*)tile);
                  CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user7, 0.0F);
                  mapMenu->hoveredMapIcon = nullptr;
               }
            };
            __declspec(naked) void Outer() {
               _asm {
                  push eax; // protect
                  push esi;
                  call Inner;
                  add  esp, 4;
                  pop  eax; // restore
                  cmp  eax, 0x2B;
                  jne  lNotAMatch;
               lNormal:
                  mov  ecx, 0x005B6CB6;
                  jmp  ecx;
               lNotAMatch:
                  mov  ecx, 0x005B6D7E;
                  jmp  ecx;
               };
            };

            void Apply() {
               WriteRelJump(0x005B6CAD, (UInt32)&Outer);
               SafeWrite32 (0x005B6CB2, 0x90909090); // courtesy NOPs
            };
         };
         namespace NavigationHook {
            //
            // When the joystick is being used to control the map, it can still send directional keypresses to 
            // the menu for keynav. We need to force the menu to ignore these keypresses. If we don't, then we 
            // can get bugs where moving the mouse off of a map icon fires a fake A-press (and where pressing a 
            // directional key while hovering over an icon can do the same). I'm not entirely sure how that 
            // happens, but my hypothesis is that we're still keyboard navigating as we move the map cursor 
            // around, and we eventually navigate onto some other tile that then misbehaves.
            //
            // Separately from this, we need to ensure that the A button works reliably.
            //
            bool HandleAPress(RE::MapMenu* mapMenu) {
               if (CALL_MEMBER_FN((RE::Tile*)mapMenu->tile, GetFloatTraitValue)(kTileValue_user21) != 2.0F)
                  return false;
               UInt8 page = CALL_MEMBER_FN(mapMenu->background, GetFloatTraitValue)(kTileValue_user0);
               if (page > 2)
                  return false;
               {
                  auto ui = RE::InterfaceManager::GetInstance();
                  if (CALL_MEMBER_FN(ui, GetTopmostMenuID)() != 1)
                     return false;
               }
               auto tile = mapMenu->hoveredMapIcon;
               if (tile) {
                  mapMenu->HandleMouseUp(CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_id), tile);
                  return true;
               }
               return true;
            };
            bool CheckSuppressNavigation(RE::MapMenu* mapMenu) {
               if (CALL_MEMBER_FN((RE::Tile*)mapMenu->tile, GetFloatTraitValue)(kTileValue_user21) != 2.0F)
                  return false;
               if (CALL_MEMBER_FN(mapMenu->background, GetFloatTraitValue)(kTileValue_user0) <= 2.0F)
                  return true;
               return false;
            };
            __declspec(naked) void Outer() {
               _asm {
                  cmp  eax, 0xE;      //
                  je   lHookedBranch; // reproduce patched-over IF
                  cmp  eax, 9;
                  je   lAButtonBranch;
                  cmp  eax, 1;
                  jl   lOtherBranch;
                  cmp  eax, 4;
                  jg   lOtherBranch;
                  //
                  // 1 <= eax <= 4; suppress directional keys if we're on a map
                  //
                  push esi;
                  call CheckSuppressNavigation;
                  add  esp, 4;
                  mov  ecx, 0x005BC43D; // end of vanilla subroutine; we'll return the return value of CheckSuppressNavigation(esi);
                  jmp  ecx;
               lAButtonBranch:
                  push esi;
                  call HandleAPress;
                  add  esp, 4;
                  mov  ecx, 0x005BC43D; // end of vanilla subroutine; we'll return the return value of HandleAPress(esi);
                  jmp  ecx;
               lOtherBranch:
                  mov  ecx, 0x005BC21C;
                  jmp  ecx;
               lHookedBranch:
                  mov  ecx, 0x005BC1D8;
                  jmp  ecx;
               };
            };

            void Apply() {
               WriteRelJump(0x005BC1D3, (UInt32)&Outer);
            };
         };
         namespace LocalMap {
            namespace SetCursorPosOnView {
               //
               // Set cursor initial position when the map is opened. The game already does this 
               // for the local cursor only, but it seems to do it incorrectly.
               //
               void Inner(RE::MapMenu* mapMenu) {
                  if (CALL_MEMBER_FN((RE::Tile*)mapMenu->tile, GetFloatTraitValue)(kTileValue_user21) != 2.0F)
                     return;
                  auto cursor = mapMenu->localCursor;
                  auto canvas = mapMenu->localMap;
                  float centerX, centerY;
                  {
                     //centerX = CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_user10);
                     //centerY = CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_user11);
                     float minX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user2);
                     float minY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user3);
                     float maxX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user4);
                     float maxY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user5);
                     centerX = (maxX - minX) / 2;
                     centerY = (maxY - minY) / 2;
                  }
                  CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_user0, centerX);
                  CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_user1, centerY);
               };
               __declspec(naked) void Outer() {
                  _asm {
                     mov  eax, 0x0058CEB0; // reproduce patched-over call
                     call eax;             //
                     push edi;
                     call Inner;
                     add  esp, 4;
                     mov  eax, 0x005BAC40;
                     jmp  eax;
                  };
               };

               void Apply() {
                  WriteRelJump(0x005BAC3B, (UInt32)&Outer);
                  //
                  // NOP out an UpdateFloat call that would overwrite our patch:
                  //
                  SafeWrite8 (0x005BAC4E, 0x90);
                  SafeWrite32(0x005BAC4F, 0x90909090);
                  SafeWrite16(0x005BAC55, 0x9090);
                  SafeWrite8 (0x005BAC57, 0x90);
                  SafeWrite32(0x005BAC58, 0x90909090);
                  SafeWrite32(0x005BAC5C, 0x90909090);
                  SafeWrite16(0x005BAC60, 0x9090);
               };
            };

            void Apply() {
               SetCursorPosOnView::Apply();
            };
         };
         namespace WorldMap {
            namespace SetCursorPosOnView {
               //
               // Set cursor initial position when the map is opened
               //
               void Inner(RE::MapMenu* mapMenu) {
                  if (CALL_MEMBER_FN((RE::Tile*)mapMenu->tile, GetFloatTraitValue)(kTileValue_user21) != 2.0F)
                     return;
                  auto cursor = mapMenu->worldCursor;
                  auto canvas = mapMenu->worldMap;
                  float centerX, centerY;
                  {
                     //centerX = CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_user10);
                     //centerY = CALL_MEMBER_FN(canvas, GetFloatTraitValue)(kTileValue_user11);
                     float minX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user2);
                     float minY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user3);
                     float maxX = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user4);
                     float maxY = CALL_MEMBER_FN(cursor, GetFloatTraitValue)(kTileValue_user5);
                     centerX = (maxX - minX) / 2;
                     centerY = (maxY - minY) / 2;
                  }
                  CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_user0, centerX);
                  CALL_MEMBER_FN(cursor, UpdateFloat)(kTileValue_user1, centerY);
               };
               __declspec(naked) void Outer() {
                  _asm {
                     mov  eax, 0x0058CEB0; // reproduce patched-over call
                     call eax;             //
                     push esi;
                     call Inner;
                     add  esp, 4;
                     mov  eax, 0x005BA498;
                     jmp  eax;
                  };
               };

               void Apply() {
                  WriteRelJump(0x005BA493, (UInt32)&Outer);
               };
            };

            void Apply() {
               SetCursorPosOnView::Apply();
            };
         };

         void Apply() {
            if (!g_xInputPatchApplied)
               return;
            _MESSAGE("[MapMenu] Installing XInput support...");
            FrameHandler::Apply();
            MouseoverHook::Apply();
            NavigationHook::Apply();
            LocalMap::Apply();
            WorldMap::Apply();
         };
      };
      namespace LocalMapInitialRenderHack {
         //
         // With NorthernUI's XML, the first time the local map renders, it's blank. Testing fails 
         // to reveal any readily identifiable discrepancy in how the NiNodes are rendered. Testing 
         // with code injection reveals that it's not a problem with MapMenu::RenderLocalMap or its 
         // immediate caller: patching both functions' callers to repeat the calls has no effect.
         //
         // It seems to be some odd nuance in how the UI is rendered... so we'll use a cheap hack: 
         // when the local map is rendered for the first time, we'll just force it to be redrawn on 
         // the next frame.
         //
         enum State : UInt8 {
            kState_NeverRendered = 0,
            kState_FirstRender,
            kState_FirstRenderAcknowledged, // HandleFrame runs after HandleClick, so we actually need to delay our response an extra frame
            kState_AfterFirstRender
         };
         static State s_state;

         namespace DetectFirstRender {
            void Inner() {
               if (s_state == kState_NeverRendered)
                  s_state = kState_FirstRender;
            };
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x0058CEB0; // reproduce patched-over call
                  call eax;             //
                  call Inner;
                  mov  eax, 0x005B8762;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005B875D, (UInt32)&Outer); // patches MapMenu::RenderLocalMap
            };
         };
         namespace UpdateOnNextFrame {
            void _stdcall Shim(RE::MapMenu* menu) {
               if (s_state == kState_FirstRender) {
                  //
                  // This is the frame on which the first render took place.
                  //
                  s_state = kState_FirstRenderAcknowledged;
               } else if (s_state == kState_FirstRenderAcknowledged) {
                  //
                  // This is the frame after the first render.
                  //
                  s_state = kState_AfterFirstRender;
                  CALL_MEMBER_FN(menu, RenderLocalMap)();
               }
            };
            __declspec(naked) bool Outer() {
               _asm {
                  push ecx;
                  call Shim; // _stdcall
                  push 0x3FF;           // reproduce patched-over call
                  mov  eax, 0x0057C140; // 
                  call eax;             //
                  add  esp, 4;          //
                  retn;
               };
            };
            void Apply() {
               WriteRelCall(0x005B70F8, (UInt32)&Outer); // patches MapMenu::HandleFrame
            };
         };
         namespace ResetOnMenuDestroyed {
            void Inner() {
               s_state = kState_NeverRendered;
            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x00401F20; // reproduce patched-over call
                  call eax;             //
                  call Inner;
                  mov  eax, 0x005B7502;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x005B74FD, (UInt32)&Outer); // patches MapMenu::~MapMenu
            };
         };
         void Apply() {
            DetectFirstRender::Apply();
            UpdateOnNextFrame::Apply();
            ResetOnMenuDestroyed::Apply();
         };
      };
      namespace MapZoomControls {
         //
         // Changing the ZOOM trait on the map image doesn't update the map size or the 
         // position of the map markers. We need to add a button that forces a redraw.
         //
         void _stdcall Inner(RE::MapMenu* menu) {
            float tab = CALL_MEMBER_FN(menu->background, GetFloatTraitValue)(RE::kTagID_user0);
            if (tab == 2.0F) { // world map
               float old_zoom = menu->GetLastComputedWorldMapZoom();
               float panX = CALL_MEMBER_FN(menu->worldMap, GetFloatTraitValue)(RE::kTagID_user10);
               float panY = CALL_MEMBER_FN(menu->worldMap, GetFloatTraitValue)(RE::kTagID_user11);
               //
               menu->UpdateWorldMapZoom();  // Force MapMenu to recalculate values that it cached in UpdateMapMenuWorldMap...
               RE::UpdateMapMenuWorldMap(); // ...and then actually redraw the map.
               //
               // Now, we need to reposition the map, since UpdateMapMenuWorldMap will have 
               // snapped the view back to the center.
               //
               float new_zoom = CALL_MEMBER_FN(menu->worldMap, GetFloatTraitValue)(RE::kTagID_zoom) / 100.0F;
               panX = panX * new_zoom / old_zoom;
               panY = panY * new_zoom / old_zoom;
               CALL_MEMBER_FN(menu->worldMap, UpdateFloat)(RE::kTagID_user10, panX);
               CALL_MEMBER_FN(menu->worldMap, UpdateFloat)(RE::kTagID_user11, panY);
            } else if (tab == 1.0F) { // local map
               RE::UpdateMapMenuLocalMap(true);
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               cmp  edi, 0x15; // reproduce patched-over branch
               je   lTakeBranch;
               cmp  edi, 9001;
               jne  lDone;
               push esi;
               call Inner; // stdcall
            lDone:
               mov  eax, 0x005BB8C6;
               jmp  eax;
            lTakeBranch:
               mov  eax, 0x005BB8B6;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005BB8B1, (UInt32)&Outer);
         };
      };

      namespace DebugHook {
         void Inner(RE::Tile* localMap) {
            _MESSAGE("MapMenu debug hook tripped!");
            auto menu = (RE::MapMenu*) CALL_MEMBER_FN(localMap, GetContainingMenu)();
            if (!menu)
               return;
            {  // LocalLayout
               if (auto tile = (RE::Tile*) menu->localLayout) {
                  _MESSAGE(" - Local layout");
                  auto node = tile->renderedNode;
                  if (node) {
                     _MESSAGE("    - Transform L: (%f, %f, %f)", node->m_localTranslate.x, node->m_localTranslate.y, node->m_localTranslate.z);
                     _MESSAGE("    - Transform W: (%f, %f, %f)", node->m_worldTranslate.x, node->m_worldTranslate.y, node->m_worldTranslate.z);
                  } else
                     _MESSAGE("    - No node");
               } else
                  _MESSAGE(" - No local layout");
            }
            {  // LocalIcons
               if (auto tile = (RE::Tile*) menu->localIcons) {
                  _MESSAGE(" - Local icons");
                  auto node = tile->renderedNode;
                  if (node) {
                     _MESSAGE("    - Transform L: (%f, %f, %f)", node->m_localTranslate.x, node->m_localTranslate.y, node->m_localTranslate.z);
                     _MESSAGE("    - Transform W: (%f, %f, %f)", node->m_worldTranslate.x, node->m_worldTranslate.y, node->m_worldTranslate.z);
                  } else
                     _MESSAGE("    - No node");
               } else
                  _MESSAGE(" - No local icons");
            }
            NiNode* node = localMap->renderedNode;
            if (node) {
               _MESSAGE(" - Local map");
               _MESSAGE("    - Local translation: (%f, %f, %f)", node->m_localTranslate.x, node->m_localTranslate.y, node->m_localTranslate.z);
               _MESSAGE("    - World translation: (%f, %f, %f)", node->m_worldTranslate.x, node->m_worldTranslate.y, node->m_worldTranslate.z);
               for (UInt32 i = 0; i < node->m_children.firstFreeEntry; i++) {
                  BSScissorTriShape* child = (BSScissorTriShape*) node->m_children.data[i];
                  if (!child)
                     break;
                  _MESSAGE("    - Child %d", i);
                  _MESSAGE("       - Local translation: (%f, %f, %f)", child->m_localTranslate.x, child->m_localTranslate.y, child->m_localTranslate.z);
                  _MESSAGE("       - World translation: (%f, %f, %f)", child->m_worldTranslate.x, child->m_worldTranslate.y, child->m_worldTranslate.z);
                  /*
                  auto geometry = child->geomData;
                  if (!geometry) {
                     _MESSAGE("       - No geometry data!");
                     break;
                  }
                  {  // Log vertices
                     UInt16 count = geometry->m_usVertices;
                     auto vertices = (NiVector3*)    geometry->m_pkVertex;
                     auto colors   = (NiColorAlpha*) geometry->m_pkColor;
                     for (UInt16 i = 0; i < count; i++) {
                        NiVector3*    vert  = &vertices[i];
                        NiColorAlpha* color = &colors[i];
                        _MESSAGE("       - Vertex %d: (%f, %f, %f) with alpha %f", i, vert->x, vert->y, vert->z, color->a);
                     }
                  }
                  //*/
               }
               {  // LocalIcons
                  auto menu = (RE::MapMenu*) CALL_MEMBER_FN(localMap, GetContainingMenu)();
                  if (menu)
                     if (auto tile = (RE::Tile*) menu->localIcons) {
                        _MESSAGE(" - Local icons");
                        node = tile->renderedNode;
                        if (node) {
                           _MESSAGE("    - Transform L: (%f, %f, %f)", node->m_localTranslate.x, node->m_localTranslate.y, node->m_localTranslate.z);
                           _MESSAGE("    - Transform W: (%f, %f, %f)", node->m_worldTranslate.x, node->m_worldTranslate.y, node->m_worldTranslate.z);
                        } else
                           _MESSAGE("    - No node");
                     } else
                        _MESSAGE(" - No local icons");
               }
            } else {
               _MESSAGE(" - No rendered node.");
            }
            _MESSAGE(" - Done logging.");
         };
         __declspec(naked) void Outer() {
            __asm {
               push ecx; // protect
               push ecx;
               call Inner;
               pop  eax; // clean up after last non-member call
               pop  ecx; // restore
               push 0xFC8; // overwritten vanilla instruction
               mov  eax, 0x005B8746;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005B8741, (UInt32)&Outer);
         };
      };

      void Apply() {
         _MESSAGE("[MapMenu] Patching...");
         LocalMapHooks::Apply();
         WorldMapHooks::SafeHoveredMapIcon::Apply();
         QuestListHooks::Apply();
         XInput::Apply();
         LocalMapInitialRenderHack::Apply();
         MapZoomControls::Apply();
      };
   }
}