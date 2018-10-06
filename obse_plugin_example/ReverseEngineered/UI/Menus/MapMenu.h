#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameMenus.h"
#include "obse/NiTypes.h"
#include "obse/Utilities.h"

namespace RE {
   class BSClearZNode;
   extern NiPointer<NiNode>* const niptrLODWaterRoot;
   extern BSClearZNode**     const niptrLODRoot;

   extern float* const mapMenuBackgroundColorR; // used only if the local map is using a background image; used as an argument 
   extern float* const mapMenuBackgroundColorG; // to NiDX9Renderer::SetClearColor4 when rendering each cell to the local map; 
   extern float* const mapMenuBackgroundColorB; // effectively controls the background color (akin to GIMP Color to Alpha).
   //
   // TODO: Make it possible to override these using uservars user21, user22, and user23 with values out of 255

   class MapMenu : public Menu { // sizeof == 0x100
      public:
         MapMenu();
         ~MapMenu();

         enum {
            kID = 0x3FF,
         };
         enum {
            kMapValue_LocationName = kTileValue_user1,
            kMapValue_Date,								// (as string)
            kMapValue_QuestListTitle,
            kMapValue_Scale,
         };
         enum {
            kTileID_Background = 9,
            kTileID_JournalScrollbar   = 11,
            kTileID_JournalScrollThumb = 12,
            kTileID_JournalPane        = 13,
            kTileID_FocusBox           = 23,
            kTileID_ShowOnMapButton    = 31,
            kTileID_JournalBackButton  = 32,
            kTileID_WorldMapContainer  = 41,
            kTileID_MapTooltip         = 42,
            kTileID_WorldMapIcon       = 43,
            kTileID_LocalMapContainer  = 46,
            kTileID_LocalMapRenderTarget  = 47,
            kTileID_LocalMapIconContainer = 48,
            kTileID_LocalMapIcon          = 49,
            kTileID_QuestListItem      = 51,
            kTileID_WorldMapXboxCursor = 61,
            kTileID_LocalMapXboxCursor = 62,
            kTileID_WorldMapWindow     = 63,
            kTileID_NorthernUI_ForceRedrawCurrentMap = 9001,
         };

         //TODO: How to get currently selected mapmarker??
         // worldIconPaper:string is marker name
         // game converts map icon coords to world coords, then walks mapmarker list
         // compares names, coords until it finds a match
         // only does this when clicking on an icon to travel, otherwise would be painfully inefficient
         // can get map coords for active icon tile from user1 and user2
         struct MapMarkerEntry {
            TESObjectREFR*  mapMarker;
            MapMarkerEntry* next;

            TESObjectREFR*  Info() const { return mapMarker; }
            MapMarkerEntry* Next() const { return next; }
         };

         TileImage* background;				// 28 // ID #9
         TileRect*  pageTabTargets[5];		// 2C .. 03C   // IDs #1 - 5
         TileImage* journalScrollBar;		// 40 // ID#11 // Scrollbar
         TileImage* journalScrollMarker;	// 44 // ID#12 // Scrollbar thumb
         TileRect*  journalPane;				// 48 // ID#13 // Scrollable pane
         TileRect*  focusBox;					// 4C // ID#23
         TileImage* journalMapButton;		// 50 // ID#31 // Button: Show quest target on the map
         TileImage* journalBackButton;		// 54 // ID#32
         TileImage* worldMap;					// 58 // ID#41
         Tile*      worldIconPaper;			// 5C // ID#42 // Hover-tip for the icon currently being hovered over
         TileImage* localLayout;				// 60 // ID#46
         Tile*      localMap;					// 64 // ID#47 // Local map is rendered to child BSScissorTriShapes injected into this tile's rendered NiNode
         Tile*      localIcons;				// 68 // ID#48
         TileImage* worldCursor;				// 6C // ID#61 // Xbox cursor for world map
         TileImage* localCursor;				// 70 // ID#62 // Xbox cursor for local map
         TileRect*  worldWindow;				// 74 // ID#63
         UInt32 unk78[3];					//078 .. 080
         UInt8  unk84;						//084 - init'd to -1
         UInt8  pad85[3];
         float  dragCursorX; // 088 // cursor X-position at last handled frame during a drag
         float  dragCursorY; // 08C // cursor Y-position at last handled frame during a drag
         SInt32 unk90;						//090
         SInt32 unk94;						//094
         UInt32 worldMapWidth;  // 98 // unk098 thru unk0AC used in converting map coords to world coords
         UInt32 worldMapHeight; // 9C // the width and height are values pulled from the worldspace, scaled according to the zoom trait on the worldMap tile
         SInt32 unkA0; // A0 // possibly world map X-min
         SInt32 unkA4; // A4 // possibly world map X-max
         SInt32 unkA8; // A8 // possibly world map Y-min
         SInt32 unkAC; // AC // possibly world map Y-max
         BSStringT destinationName;			// 0B0 updated only when clicking on a marker, not on mouseover
         float  targetX;					// 0B8 x, y of selected map marker icon (NOT world coords)
         float  targetY;					// 0BC these only updated on click
         UInt32 unkC0;						// 0C0
         MapMarkerEntry* mapMarkers;				//0C4 // TODO: Change to RE::LinkedPointerList<TESObjectREFR>
         void*  unkC8; // C8 // LinkedPointerList<???>*?
         void*  unkCC; // CC // LinkedPointerList<???>* for quest objectives?
         TESWorldSpace* worldSpace;				//0D0
         float  unkD4; // D4 // x
         float  unkD8; // D8 // y
         UInt8  unkDC; // DC // awaiting message menu callback for moving the local map custom marker?
         UInt8  unkDD; // DD
         UInt8  unkDE; // DE
         UInt8  unkDF; // DF
         TileImage* unkE0; // E0 // A world or local map icon? Those are templated, but they're the only image tiles that use user8.
         float  dragStartWorldMapCropX; // E4 // cropX of this->worldMap
         float  dragStartWorldMapCropY; // E8 // cropY of this->worldMap
         UInt32 unkEC; // EC
         UInt32 unkF0; // F0
         Tile*      hoveredMapIcon; // F4 // generated world map icon that we're mousing over // (same for local? verify)
         TileImage* unkF8; // F8 // player's you-are-here marker (for the world map, at least; not sure about local)
         TileImage* unkFC; // FC

         void  ForceDismissTooltip();
         float GetLastComputedWorldMapZoom(); // inexact
         const char* GetSelectedMarkerName();
         TESObjectREFR* GetSelectedMarker();
         void UpdateMarkerName(TESObjectREFR* mapMarker, const char* newName);
         void UpdateWorldMapZoom();

         MEMBER_FN_PREFIX(MapMenu);
         DEFINE_MEMBER_FN(Destructor,      void, 0x005B7490);
         DEFINE_MEMBER_FN(PanMap,          void, 0x005B67F0, float panX, float panY, bool isWorldMap);
         DEFINE_MEMBER_FN(RenderLocalMap,  void, 0x005B7550);
         DEFINE_MEMBER_FN(SetMenuTabIndex, void, 0x005BB210, UInt32 tabIndex, void* maybeUnused);
   };
   static_assert(sizeof(MapMenu) <= 0x100, "RE::MapMenu is too large!");
   static_assert(sizeof(MapMenu) >= 0x100, "RE::MapMenu is too small!");
   static_assert(offsetof(MapMenu, hoveredMapIcon) <= 0xF4, "RE::MapMenu::hoveredMapIcon is too late!");
   static_assert(offsetof(MapMenu, hoveredMapIcon) >= 0xF4, "RE::MapMenu::hoveredMapIcon is too early!");

   DEFINE_SUBROUTINE_EXTERN(void, UpdateMapMenuLocalMap, 0x005BA4D0, bool);
   DEFINE_SUBROUTINE_EXTERN(void, UpdateMapMenuWorldMap, 0x005B91E0);
};