#pragma once
#include "ReverseEngineered/_BASE.h"

class Menu;
class NiAlphaProperty;
class NiDirectionalLight;
class NiNode;
class SceneGraph;
class ShadowSceneNode;
class TESObjectREFR;
class Tile;

namespace RE {
   class InterfaceManager;
   class Menu;
   class Tile;

   extern SInt32* const SInt32ScreenWidth;
   extern SInt32* const SInt32ScreenHeight;
   extern bool*   const bPlayerLevelUpIsQueued;
   extern float*  const fNormalizedScreenWidthF;
   extern float*  const fNormalizedScreenHeightF;
   extern double* const fNormalizedScreenWidthDbl;
   extern double* const fNormalizedScreenHeightDbl;
   extern InterfaceManager** const ptrInterfaceManager;

   // In Bethesda's code, this constant is a double at 00A68FD0, though I can't be sure it isn't in other places as well.
   const float depthToUnits = -0.008; // convert DEPTH trait values to NiNode units

   // Character tables used for ScancodeToChar.
   //char* kCharacterTables[256][5] = (char*) 0x00B02198;

   // If the screen width exceeds the screen height, then the screen is scaled so that the 
   // UI is 960 normalized pixels high. Otherwise, the screen is scaled so that the UI is 
   // 1280 normalized pixels wide.
   //
   DEFINE_SUBROUTINE_EXTERN(float,  GetNormalizedScreenWidth,  0x0057D7A0);
   DEFINE_SUBROUTINE_EXTERN(float,  GetNormalizedScreenHeight, 0x0057D7F0);
   DEFINE_SUBROUTINE_EXTERN(float,  GetNormalizedScreenMinX,   0x0057E2D0); // Given (0, 0, 0) as the screen center.
   DEFINE_SUBROUTINE_EXTERN(float,  GetNormalizedScreenMaxX,   0x0057E330); //
   DEFINE_SUBROUTINE_EXTERN(float,  GetNormalizedScreenMinY,   0x0057E390); //
   DEFINE_SUBROUTINE_EXTERN(float,  GetNormalizedScreenMaxY,   0x0057E3F0); //
   //
   DEFINE_SUBROUTINE_EXTERN(float,  GetSafeZoneX, 0x0057D330); // The "safe zone" is defined by two pairs of INI settings -- one for 4:3 
   DEFINE_SUBROUTINE_EXTERN(float,  GetSafeZoneY, 0x0057D390); // and one for widescreen.
   //
   DEFINE_SUBROUTINE_EXTERN(void,   CancelMenuFade,          0x00583CF0, Menu* menu); // uses unk130
   DEFINE_SUBROUTINE_EXTERN(void,   FadeMenu,                0x00584300, Menu* menu, float fadeTime); // not sure whether this fades in or fades out // uses unk130
   DEFINE_SUBROUTINE_EXTERN(float,  GetMenuFadeElapsedRatio, 0x00583CF0, Menu* menu); // uses unk130 // returns value from 0.0 to 1.0, with -1.0 as an error value for fades with a bad (negative) time
   //
   DEFINE_SUBROUTINE_EXTERN(float,  GetNewMenuDepth,                   0x00584980); // returns highest menu depth + 2 (suitable for use on a new menu); also updates cursor to be 100 depth-values in front of highest menu
   DEFINE_SUBROUTINE_EXTERN(UInt32, GetTopmostMenuIDUnderUnkCondition, 0x00578FE0);
   DEFINE_SUBROUTINE_EXTERN(void,   PlayUIClicksound,                  0x0057DE50, UInt32 index);
   DEFINE_SUBROUTINE_EXTERN(void,   SetInterfaceManagerCursorAlpha,    0x00583DF0, UInt32 alpha); // alpha out of 255
   //
   DEFINE_SUBROUTINE_EXTERN(void,   HideBigFour,          0x0057CC00);
   DEFINE_SUBROUTINE_EXTERN(void,   ShowOrRefreshBigFour, 0x0057CAC0);

   class InterfaceManager {
      public:
         InterfaceManager();
         ~InterfaceManager();

         enum { // special values for IsActiveMenuMode()
            kMenuMode_GameMode = 0,
            kMenuMode_BigFour,
            kMenuMode_Any,
            kMenuMode_Console,
         };
         enum {
            kSpecialKey_Backspace  = 0x80000000,
            kSpecialKey_ArrowLeft  = 0x80000001,
            kSpecialKey_ArrowRight = 0x80000002,
            kSpecialKey_ArrowUp    = 0x80000003,
            kSpecialKey_ArrowDown  = 0x80000004,
            kSpecialKey_Home       = 0x80000005,
            kSpecialKey_End        = 0x80000006,
            kSpecialKey_Del        = 0x80000007,
            kSpecialKey_Newline    = 0x80000008,
            kSpecialKey_PageUp     = 0x80000009,
            kSpecialKey_PageDown   = 0x8000000A,
         };

         struct Fade {
            //
            // Actually, I think this might be wrong. It's certainly related to menu fades, but it 
            // may handle more functionality. See, FadeMenu above is called by Tile::AnimateTrait.
            //
            Menu*  menu;  // 00
            float  unk04 = 0.0; // 04 // time elapsed?
            float  unk08; // 08 // time maximum? // initialized to max(0.0, menufade or explorefade)
            Fade*  next;  // 0C
            Fade*  prev;  // 10
         };

         SceneGraph* unk000; // 000 // == *g_interfaceSceneGraph
         SceneGraph* unk004; // 004 // == *g_interface3DSceneGraph // has a BSFogProperty added to it
         UInt8  unk008; // 08
                        //  - truthy means not in "menu mode" (so, "in explore mode?")
                        //  - value 1 alters the behavior of ShowQuickKeysMenu
                        //  - value 2 and value 5 are processed the same way by InterfaceManager::Upate
                        //  - value 3 is set when a menu ID is being added to the active stack
                        //  - value 4 is checked for at 0x00584267
         UInt8  unk009; // 09 // set to 1 at 0x00584094 and 0x00584189; possibly indicates that a menu fade is in progress
         UInt8  statsMenuSelectedTab; // 0A // verified via memory inspection during testing
         UInt8  itemsMenuSelectedTab; // 0B // verified via memory inspection during testing
         UInt8  magicMenuSelectedTab; // 0C // verified via memory inspection during testing
         UInt8  mapMenuSelectedTab; // 0D
                                    // - map menu tab index
                                    // - found via disassembly; verified via memory inspection
                                    // - ...but tampering with it doesn't seem to affect anything??
                                    // - remembered between menu closure/reopening
                                    // - if it's equal to 0xFF, we pull the tab index from the map menu's background tile's user0 trait
                                    // - if whatever signed value we end up with is greater than 5, it's clamped to 5
         UInt8  unk00E; // 0E
         UInt8  unk00F; // 0F
         UInt32  activatePickLength; // 10 // set at 0067176B
         UInt32  unk014[(0x018 - 0x014) >> 2];	// 014
         NiDirectionalLight* playerSceneLight; // 018
         Tile*   cursor; // 1C
         NiVector3 cursorPos; // 20 // values are in normalized pixels and (0, 0, 0) represents the screen center (Y is the 3D depth; Z is vertical); add the normalized screen half-width and half-height for proper X/Z coords
         float   unk02C; // 2C // related to cursorPos
         UInt32  unk030; // 30
         float   unk034; // 34 // related to the y-axis
         float   mouseZAxisMovement; // 38 // scroll wheel
         float   mouseLMBDownTime;   // 3C // the length of time that the left mouse button has been held down, in seconds
         SInt32  unk040; // 40 // related to the x-axis; see DialogMenu::HandleMouseDown
         SInt32  unk044; // 44 // related to the y-axis; see DialogMenu::HandleMouseDown
         SInt32  unk048; // 48 // tile ID; see DialogMenu::HandleMouseDown
         Tile*   unk04C; // 4C // tile;    see DialogMenu::HandleMouseDown
         bool    debugTextOn;					// 050
         UInt8   unk051[3];
         NiNode* mainScene;   // 054 // "Main Root"
         NiNode* cursorScene; // 058 // "Cursor Root"
         NiNode* unk05C; // 05C
         NiNode* unk060; // 060 // containing node for player model in menus?
         ShadowSceneNode* playerScene; // 064
         Tile*   menuRoot; // 068
         Tile*   strings;  // 06C
         NiNode* unk070;							// 070 // "DebugText Root"
         UInt32  unk074;							// 074
         NiAlphaProperty* unk078; // 078 NiAlphaProperty* // gets added to all TileImage and Tile3D NiNodes (they share it)
         UInt32  unk07C; // 07C // byte gets set to 1 when any TileImage or Tile3D successfully generates a NiNode?
         Tile*   hudReticle;					// 080
         UInt32  unk084;							// 084
         Tile*   altActiveTile; // 088 // preferred over activeTile for keyboard navigation; allows decoupling keyboard/gamepad focus from mouse focus
         UInt32  unk08C;        // 08C // int; related to altActiveTile xdefault processing; value itself can be used as an executable-set xdefault
         UInt32  unk090;							// 090 // set to 1 when dropping an item from the inventory
         UInt32  unk094;							// 094
         Tile*   activeTile; // 098 // moused-over tile
         Menu*   activeMenu; // 09C // menu over which the mouse cursor is placed
         Tile*   unk0A0;							// 0A0 // related to Menu events 06, 07, and 09, but Bethesda never used them
         Menu*   unk0A4;							// 0A4 // containing Menu for unk0A0
         UInt32  unk0A8;							// 0A8
         UInt32  unk0AC;							// 0AC
         UInt8   msgBoxButtonPressed; // B0 // zero-indexed; -1 means none pressed
         UInt8   unk0B1;						// 0B1
         UInt8   unk0B2;
         UInt8   unk0B3;
         void*   unk0B4;						// 0B4 - stores callback for ShowMessageBox() (anything else?)
         UInt8   unk0B8; // B8
         bool    unk0B9 = 1; // B9 // very likely a "cursor is active" bool
         UInt8   unk0BA;
         UInt8   unk0BB;
         TESObjectREFR* debugSelection; // 0BC // the current console ref
         UInt32  unkC0;
         TESObjectREFR* unk0C4; // 0C4
         TESObjectREFR* unk0C8; // 0C8
         TESObjectREFR* unk0CC; // 0CC
         UInt32  unk0D0; // D0
         UInt32  unk0D4; // D4
         UInt32  unk0D8; // D8
         UInt32  unk0DC; // DC
         UInt32  activeMenuIDs[9]; // E0 ... 104 // lists open menus with certain stackingtypes, in order from bottom to top // the "big four" menus are represented as 1, not with their IDs; empty slots are zero
         UInt32  unk104; // 104 // menu ID
         void*   unk108; // struct, size 0xC bytes; no VTBL or RTTI; constructor at 00538B20
         UInt32  playerQueuedToGoToJail; // 10C // if truthy, player is queued to go to jail and will be sent there by InterfaceManager::Update; this field then gets reset to 0
         UInt32  unk110; // if truthy, a call to PlayerCharacter::00670CA0 is queued and will be run by InterfaceManager::Update; this field then gets reset to 0
         bool    inputIsUppercase; // 114 // contingent on Shift keys
         UInt8   unk115;
         UInt8   unk116;
         UInt8   unk117;
         UInt32  unk118; // 118 // special character code
                                //  - if sign bit is set, see kSpecialKey enum
                                //  - else:
                                //     - 1 = Alt
                                //     - 2 = Ctrl
                                //     - 4 = Shift
         UInt32  unk11C; // 11C // can be any kSpecialKey enum that is valid for a one-line textbox? // could be the arrow key we're repeating?
         UInt32  repeatableKeyStartTime; // 120 // milliseconds: when we first pressed an arrow key down?
         UInt32  lastRepeatableKeyFire;  // 124 // milliseconds: last time we fired a keypress for an arrow key being held down
         UInt32  unk128;
         UInt32  unk12C;
         Fade*   unk130; // 130 // linked list of menus being faded, and fade state?
         UInt32  unk134;

         bool  CreateTextEditMenu(const char* promptText, const char* defaultText);
         float GetDepth();
         bool  IsGameMode();
         bool  MenuModeHasFocus(UInt32 menuType);		// returns true if menuType is on top (has focus)
         bool  OtherMenusAreActive(UInt32 yourMenuID);
         void  SetCursorPosition(float x, float y, bool forceShowCursor = false);

         static InterfaceManager* GetInstance() {
            const auto _f = (InterfaceManager*(*)(bool canCreateNew, bool)) 0x00582160;
            return _f(0, 1);
         };
         static float GetNormalizedMouseAxisMovement(UInt8 axis); // 1, 2, 3 = x, y, z

         static void ToggleDebugText();

         MEMBER_FN_PREFIX(InterfaceManager);
         DEFINE_MEMBER_FN(ActiveMenuIDsInclude,         bool,   0x0057CFB0, UInt32 menuID); // checks if a menu is active and modal
         DEFINE_MEMBER_FN(GetNewMenuDepth,              float,  0x00588890); // just wraps non-member GetNewMenuDepth
         DEFINE_MEMBER_FN(GetCursorScreenX,             SInt32, 0x005952D0); // converts cursorPos from center-relative to top-left-relative or somesuch
         DEFINE_MEMBER_FN(GetCursorScreenY,             SInt32, 0x00593020); // converts cursorPos from center-relative to top-left-relative or somesuch
         DEFINE_MEMBER_FN(GetTopmostMenuID,             UInt32, 0x0057CF60); // checks activeMenuIDs
         DEFINE_MEMBER_FN(MaintainReticleTarget,        void,   0x005806D0); // raycasts; supplies target to HUDReticle and HUDInfoMenu
         DEFINE_MEMBER_FN(Mouseout,                     void,   0x0057D730, bool fullProcess); // clears activeTile and activeMenu // "full processing" here means updating "mouseover" on the tile and firing a mouseout event on the menu
         DEFINE_MEMBER_FN(SelectItemsMenuTabByIndex,    void,   0x0057CE20, UInt8);
         DEFINE_MEMBER_FN(SelectMagicMenuTabByIndex,    void,   0x0057CE60, UInt8);
         DEFINE_MEMBER_FN(SelectMapMenuTabByIndex,      void,   0x0057CEA0, UInt8);
         DEFINE_MEMBER_FN(SelectStatsMenuTabByIndex,    void,   0x0057CDE0, UInt8);
         DEFINE_MEMBER_FN(SendKeynavEventTo,            void,   0x0057F9F0, Tile* target, UInt32 traitID, UInt32 keynavTrait); // send keynav event (kTileValue_mouseover or kTileValue_clicked) to (target). If (target) is nullptr and (traitID) is mouseover, effectively unfocuses everything. if (keynavTrait) is non-zero, it's the trait that "sent" the event (e.g. xup, xbuttona).
         DEFINE_MEMBER_FN(SetCursorFilename,            void,   0x0057DA20, const char* filename, bool modifyFlagsSomehow);
         DEFINE_MEMBER_FN(SetHUDReticleVisibility,      void,   0x0057D940, UInt8); // arg: 0, 1, 2 == force hide, force show, toggle
         DEFINE_MEMBER_FN(SetSafeZoneSize,              void,   0x0057FDC0, SInt32 x, SInt32 y); // writes to the INI values in memory if x > 0; uses x for both axes if y <= 0
         DEFINE_MEMBER_FN(SetSafeZoneState,             void,   0x0057FDC0, UInt32);
         DEFINE_MEMBER_FN(Subroutine0057CFE0,           SInt32, 0x0057CFE0, UInt32 menuID, bool);
         DEFINE_MEMBER_FN(Subroutine0057D240,           bool,   0x0057D240, UInt32);
         DEFINE_MEMBER_FN(Subroutine0057DA90,           Tile*,  0x0057DA90, UInt32* out, Tile* searchRoot); // getter; see file in _DOCS\
         //
         // Things you should probably never need to call:
         //
         DEFINE_MEMBER_FN(AddMenuToActiveIDStack,       void,   0x0057D640, UInt32 id);
         DEFINE_MEMBER_FN(ForceCloseAllActiveMenus,     void,   0x0057CEE0); // acts on activeMenuIDs: gets Menus referred to by list and calls Dispose on them; empties list; note that other pointers to the menus become unsafe, obv
         DEFINE_MEMBER_FN(HandleNavigationKeypress,     void,   0x00580BA0, UInt32 specialKeycode); // internal handler: sends menu event 0E and handles x-navigation traits // see enum below
         DEFINE_MEMBER_FN(InitializeMenuRootAndStrings, UInt32, 0x00581CC0, UInt32); // creates cursor, menuRoot, and strings
         DEFINE_MEMBER_FN(ProcessRepeatableKey,         void,   0x0057DC60, UInt32);
         DEFINE_MEMBER_FN(SendMouseoverToSafeTile,      void,   0x0057FD60); // gets a tile based on listindex and xdefault, and fires a mouseover event on it
         DEFINE_MEMBER_FN(Update,                       void,   0x005821F0); // called from multiple places including the game's main loop (indirectly); probably shouldn't call it ourselves
         DEFINE_MEMBER_FN(UpdateCursorState,            void,   0x0057E7C0); // only called by Update; shouldn't ever call it ourselves

         enum {
            kNavigationKeypress_UnkNeg2 = -2, // acts like kNavigationKeypress_XboxA, but on the menu's root tile rather than on the tile referred to by a <ref/> operator
            kNavigationKeypress_Up      =  1,
            kNavigationKeypress_Down, // also newline
            kNavigationKeypress_Right,
            kNavigationKeypress_Left,
            kNavigationKeypress_XboxStart = 5,
            kNavigationKeypress_XboxA = 9, // HandleNavigationInput always simulates a click when given this
            kNavigationKeypress_XboxB = 10,
            kNavigationKeypress_XboxX = 11,
            kNavigationKeypress_XboxY = 12,
            kNavigationKeypress_XboxLT = 13, // also shift + arrow left
            kNavigationKeypress_XboxRT = 14, // also shirt + arrow right
            kNavigationKeypress_XboxLB = 15,
            kNavigationKeypress_XboxRB = 16,
         };
   };
   static_assert(sizeof(InterfaceManager) <= 0x138, "RE::InterfaceManager is too large!");
   static_assert(sizeof(InterfaceManager) >= 0x138, "RE::InterfaceManager is too small!");
   static_assert(offsetof(InterfaceManager, menuRoot) <= 0x68, "RE::InterfaceManager::menuRoot is too late!");
   static_assert(offsetof(InterfaceManager, menuRoot) >= 0x68, "RE::InterfaceManager::menuRoot is too early!");
   static_assert(offsetof(InterfaceManager, activeMenu) <= 0x9C, "RE::InterfaceManager::activeMenu is too late!");
   static_assert(offsetof(InterfaceManager, activeMenu) >= 0x9C, "RE::InterfaceManager::activeMenu is too early!");
   static_assert(offsetof(InterfaceManager, msgBoxButtonPressed) <= 0xB0, "RE::InterfaceManager::msgBoxButtonPressed is too late!");
   static_assert(offsetof(InterfaceManager, msgBoxButtonPressed) >= 0xB0, "RE::InterfaceManager::msgBoxButtonPressed is too early!");
   static_assert(offsetof(InterfaceManager, debugSelection) <= 0xBC, "RE::InterfaceManager::debugSelection is too late!");
   static_assert(offsetof(InterfaceManager, debugSelection) >= 0xBC, "RE::InterfaceManager::debugSelection is too early!");
   static_assert(offsetof(InterfaceManager, activeMenuIDs) <= 0xE0, "RE::InterfaceManager::activeMenuIDs is too late!");
   static_assert(offsetof(InterfaceManager, activeMenuIDs) >= 0xE0, "RE::InterfaceManager::activeMenuIDs is too early!");
   static_assert(offsetof(InterfaceManager, unk108) <= 0x108, "RE::InterfaceManager::unk108 is too late!");
   static_assert(offsetof(InterfaceManager, unk108) >= 0x108, "RE::InterfaceManager::unk108 is too early!");
   static_assert(offsetof(InterfaceManager, playerQueuedToGoToJail) <= 0x10C, "RE::InterfaceManager::playerQueuedToGoToJail is too late!");
   static_assert(offsetof(InterfaceManager, playerQueuedToGoToJail) >= 0x10C, "RE::InterfaceManager::playerQueuedToGoToJail is too early!");

   //
   // Scattered notes:
   //
   //  - If you need to know whenever "active" menus -- that is, those that receive keynav events -- open and 
   //    close, hook these:
   //
   //    InterfaceManager::AddMenuToActiveIDStack
   //    InterfaceManager::ForceCloseAllActiveMenus
   //    InterfaceManager::Subroutine0057CFE0       // removes from the activeMenuIDs stack
   //
   //    Could be others; search 0xE0 in the executable; see what fields are accessed roughly in the viciinity 
   //    of InterfaceManager member functions.
   //
};