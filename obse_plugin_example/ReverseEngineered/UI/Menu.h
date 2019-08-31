#pragma once
#include "obse/GameTiles.h"
#include "ReverseEngineered/_BASE.h"

namespace RE {
   DEFINE_SUBROUTINE_EXTERN(UInt8, GetAndResetMessageButtonPressed, 0x00578D70);
   //
   // Trait aliases for Bethesda prefabs:
   //
   enum {
      kButtonTraitValue_Label     = kTileValue_user0, // some menus set this (typically to game settings)
      kButtonTraitValue_Enabled   = kTileValue_user1, // some menus check this
      kButtonTraitValue_FontIndex = kTileValue_user2,
      kButtonTraitValue_XboxSkin  = kTileValue_user20,
   };
   enum {
      kFocusBoxTraitValue_DepthBackwardOffset = kTileValue_user15,
   };
   enum {
      kScrollbarTraitValue_MinScroll       = kTileValue_user1, // 0xFAF
      kScrollbarTraitValue_MaxScroll       = kTileValue_user2, // 0xFB0
      kScrollbarTraitValue_ArrowClickStep  = kTileValue_user3, // 0xFB1
      kScrollbarTraitValue_ThumbClickStep  = kTileValue_user4, // 0xFB2
      kScrollbarTraitValue_InitialScroll   = kTileValue_user5, // 0xFB3 // some scroll behaviors directly modify this, to bypass XML-side scroll position math
      kScrollbarTraitValue_ThumbTileID     = kTileValue_user6, // 0xFB4
      kScrollbarTraitValue_CurrentScroll   = kTileValue_user7, // 0xFB5
      kScrollbarTraitValue_VisibleCount    = kTileValue_user8, // 0xFB6 // number of items visible at once
      kScrollbarTraitValue_ManualStep      = kTileValue_user9, // 0xFB7 // poorly explained; can be zero; refers to dragging?
      kScrollbarTraitValue_HideIfEmptyList = kTileValue_user10, // 0xFB8 // XML-side scroll position math also uses this, though; are we sure it does what Bethesda's comments said it did?
   };
   enum {
      kScrollThumbTraitValue_DragInterval = kTileValue_user8, // 0xFB6 // XML-defined
      kScrollThumbTraitValue_Unk09        = kTileValue_user9, // 0xFB7 // input from executable, when dragging: cursor Y-offset from scrollbar top, divided by drag interval
   };

   class Tile;
   struct TileTemplate;
   class TileMenu;
   class Menu { // sizeof == 0x28
      public:
         Menu();
         ~Menu();

         //
         // TIPS:
         //
         //  - If we want to create a custom Menu subclass, we should patch Oblivion's dynamic-cast function 
         //    at 009832E6 and have our patch check what RTTI we're casting to or from. If either RTTI is the 
         //    Menu RTTI, we should run our own dynamic-cast and return the result; otherwise, we should jump 
         //    back to vanilla. This will ensure that our Menu subclasses behave properly if the vanilla code 
         //    dynamic-casts them.
         //
         //     - If we want our menus to have their own IDs, then we need to expand the capacity of the 
         //       g_tileMenu NiTArray. Its capacity is set at 0x00587B92 (the containing subroutine may set 
         //       it twice, redundantly).
         //
         //  - No menus implement events 06 or 07, and I don't think any would implement 09 either.
         //
         //  - Menus with scrollbars tend to handle dragging the same way in the HandleFrameMouseDown event.
         //
         //  - When the left mouse button is pressed down, MouseDown and FrameMouseDown are both sent. 
         //    FrameMouseDown is sent for every frame that the button is kept help down. Technically it's 
         //    actually a MouseHold event, but I wanted the clearest name possible.
         //
         //  - To do something every frame, use HandleFrame.
         //
         //  - TODO: Does HandleKeyboardInput get special charcodes for backspace and other editing keys? 
         //    Because it IS able to pass those to MenuTextInputState::HandleKeypress...
         //

         enum NavigationInput : UInt32 { // TODO: This is the same as InterfaceManager's kNavigationKeypress enum.
            kNavInput_Up         = 1,
            kNavInput_Down       = 2, // or newline key, if mouse cursor isn't active
            kNavInput_Left       = 4,
            kNavInput_Right      = 3,
            kNavInput_XButtonA   = 9,   // gamepad? LockPickMenu checks for it // possibly "A" button
            kNavInput_Unk0B      = 0xB, // gamepad? LockPickMenu checks for it; triggers auto-attempt
            kNavInput_ShiftLeft  = 0xD, // or gamepad LT
            kNavInput_ShiftRight = 0xE, // or gamepad RT
         };

         //
         // Typical order for events:
         //  - InterfaceManager updates its cursor position and unkB9
         //  - HandleMouseout           (altActiveTile)
         //  - Unk_07                   (old unkA4)
         //  - Unk_06                   (new unkA4)
         //  - HandleMouseUp            (activeTile)     // if mouse released // "clicked" and "clickcountafter" traits are handled first
         //  - HandleMouseout           (old activeTile)
         //  - HandleMouseover          (new activeTile)
         //  - HandleMouseDown          (activeTile)     // if mouse pressed
         //  - HandleFrameMousedown     (activeTile)     // if mouse pressed this frame or still held
         //  - Unk_09                   (unkA4)          // if mouse pressed this frame or still held
         //  - HandleFrameMouseWheel    (activeTile)     // only if mouse not down
         //  - HandleKeyboardInput      (topmost menu)
         //  - HandleNavigationKeypress (topmost menu)   // only if HandleKeyboardInput returned false
         //  - Console key handling     [no events]
         //  - Pause key handling       [no events]
         //  - HandleFrame              (all menus)
         //  - "Big four" menu handling [no events]
         //

         virtual void	Dispose(bool); // 00 // destructor
         virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile); // 01 // fired by Tile::HandleTraitChange
         virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target); // 02 // fired when LMB is first pressed down
         virtual void	HandleMouseUp  (SInt32 tileID, RE::Tile* target); // 03 // fired when LMB is released // target can be nullptr; for keyboard navigation, it's the tile that HAD the <ref/> operator as opposed to the tile TARGETED BY the <ref/> operator
         virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target); // 04
         virtual void	HandleMouseout (SInt32 tileID, RE::Tile* target); // 05 // not sure whether target is the Tile that lost the mouse or the Tile that took it
         virtual void	Unk_06(SInt32 tileID, RE::Tile* tileMain, RE::Tile* tileOther); // 06
            // - fired at 00582517
            // - no menus in the game implement this event
            // - 06 and 07 form a pair that catch the start and end of some interaction
            //    - the current menu and tile for this interaction is tracked as InterfaceManager::unkA4 and InterfaceManager::unkA0
            // - 06 appears to fire when LMB is down and the cursor has moved off of tileOther and onto tileMain
            //    - InterfaceManager::unkB9 is also a condition, but that seems to be a condition for most/all mouse events and may be a "mouse enabled" bool

         virtual void	Unk_07(SInt32 tileID, RE::Tile* tileMain, RE::Tile* tileOther); // 07
            // - fired at 005824D0
            // - no menus in the game implement this event
            // - 06 and 07 form a pair that catch the start and end of some interaction
            //    - the current menu and tile for this interaction is tracked as InterfaceManager::unkA4 and InterfaceManager::unkA0
            // - 07 appears to fire when IMB is down and the cursor has moved off of tileMain and onto tileOther
            //    - InterfaceManager::unkB9 is also a condition, but that seems to be a condition for most/all mouse events and may be a "mouse enabled" bool

         virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target); // 08 // Not given mouse data; instead, query InterfaceManager::cursorPos
         virtual void	Unk_09(SInt32 tileID, RE::Tile* target); // 09
            // - fired at 00583466
            // - no menus in the game implement this event
            // - appears to be another frame-mousedown handler, but for InterfaceManager::unkA4 instead

         virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target); // 0A // Not given mouse data; instead, query InterfaceManager::mouseZAxisMovement
         virtual void	HandleFrame(); // 0B // fired at 005839A0 on all menus, unconditionally
         virtual bool	HandleKeyboardInput(UInt32 inputChar); // 0C // for keyboard shortcuts, return true if handled; returning false means the game engine catches the key // the argument is either a text char, or part of the InterfaceManager::kSpecialKey_... enum.
         virtual UInt32	GetID(); // 0D
         virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg2); // 0E
            // - return true to cancel the default action for navigation keys (e.g. if the 
            //   user is operating a textbox and you don't want the arrow keys to punt them 
            //   out of your textbox and into some other tile).
            // 
            // - calls at 00580BF5 and 00580C85

         struct Unk08 { // template?
            TileTemplate* unk00; // 00
            Unk08* next = nullptr; // 04
         };
         enum Unk24 : UInt32 {
            kMenuState_Unk1 = 1, // set at 0x00584166 // presumably "visible and active"
            kMenuState_Unk2 = 2, // closing?
            kMenuState_Unk4 = 4, // set at 0x0058401B: value 2 changes to 4 if the elapsed fade ratio is +/- 1.0
            kMenuState_Unk8 = 8, // fading in or out?
         };

         //	void** _vtbl; // 00
         TileMenu* tile  = nullptr; // 04
         Unk08     unk08;           // 08 // linked list
         Tile*     unk10 = 0;       // 10 // RepairMenu sets this to the last tile generated via templates
         UInt32    activeId = 0;    // 14 // menu ID if the stacking type == 102.0 (&no_click_past;) or >= 103.0 (&mixed_menu; or &does_not_stack;) // this is a menu's ID within InterfaceManager::activeMenuIDs
            // see TileMenu::UpdateField and Menu::RegisterTile for further information
         SInt32    maxDepth = 0; // 18 // highest absolute depth coordinate (in tile units, not 3D) among all tiles in the menu; updated whenever any tile's depth trait changes
         UInt32    unk1C = 1; // 1C // initialized to 1
         //
         UInt32    id;        // 20 // uninitialized
         UInt32    unk24 = 4; // 24 // state enum // initialized to 4, is 8 if enabled? // events 06 and 07 are only sent if this is 1

         MEMBER_FN_PREFIX(Menu);
         DEFINE_MEMBER_FN(CreateTemplatedTile, Tile*, 0x00585410, Tile* appendTo, const char* templateName, Tile* existingTemplatedTileUsuallyNull);
         DEFINE_MEMBER_FN(EnableMenu,          void,  0x00585190, UInt32);
         DEFINE_MEMBER_FN(FadeIn,              void,  0x00584390); // fades the menu in?
         DEFINE_MEMBER_FN(FadeOut,             void,  0x00584740); // fades the menu out?
         DEFINE_MEMBER_FN(PlaySoundByEditorID, void,  0x005AFD50, const char* editorID); // technically this is just LockPickMenu, but it's usable by anything
         DEFINE_MEMBER_FN(RegisterTile,        void,  0x00584880, Tile* root); // sets up the menu's root tile
         DEFINE_MEMBER_FN(ToggleOnOffLabel,    void,  0x0059B640, Tile* target, bool isOn); // technically this could be called non-member

         // Helpers for implementing your own menus:
         void Close();
         static void HandleFocusBox(Tile* focusBox, Tile* target = nullptr); // focusBox cannot be nullptr

         // Helpers for scrollbars.
         // 
         // There are no handlers for clicking the scrollbar. The scrollbar prefab's XML actually 
         // handles that on its own, by checking the "clicked" trait and taking advantage of some 
         // weirdness within the trait operator system! (Turns out, if your trait uses operators 
         // but the first operator isn't COPY, then the first operator ends up modifying the last 
         // computed value for the trait, whenever the trait is actually recomputed. You can use 
         // that to store and remember values purely within the XML.)
         // 
         // There are three different behaviors for dragging the scrollbar thumb. I mostly under-
         // stand how they update the scroll position, but I don't understand what (visible) diff-
         // erent behaviors they're meant to produce, or why Bethesda *has* three different behav-
         // iors.
         //
         enum ScrollDragBehavior : UInt32 {
            kScrollDragBehavior_Interval,  // DialogMenu, InventoryMenu
            kScrollDragBehavior_ClickStep, // MapMenu (quest list)
            kScrollDragBehavior_Advanced,  // StatsMenu (all lists)
         };
         static void HandleScrollbarThumbDrag (Tile* bar, Tile* thumb, ScrollDragBehavior, bool isHorizontal = false); // call from Menu::HandleFrameMouseDown if the event target is the scrollbar thumb
         static void HandleScrollbarMouseWheel(Tile* thumb); // call from Menu::HandleFrameMouseWheel
   };
};