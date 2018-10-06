#pragma once
#include "obse/GameForms.h"

namespace RE {
   class TESWorldSpace : public TESForm { // sizeof == 0xE0
      public:
         TESWorldSpace();
         ~TESWorldSpace();

         enum {
            // "Can't Wait" flag does not appear to be saved by editor
            kFlag_SmallWorld = 1 << 0,
            kFlag_NoFastTravel = 1 << 1,
            kFlag_OblivionWorld = 1 << 2,
            kFlag_NoLODWater = 1 << 4,
         };

         // bases
         TESFullName	fullName;	// 018
         TESTexture	texture;	// 024

         struct Unk84 {
            UInt32 width;  // 00
            UInt32 height; // 04
            UInt16 unk08;  // 08 // divide by 4096 to get the cell count X
            UInt16 unk0A;  // 0A // divide by 4096 to get the cell count Y
         };

                              // members
         NiTPointerMap<TESObjectCELL>	* cellMap;	// 030 - key is coordinates of cell: (x << 16 + y)
         TESObjectCELL	* unk034;			// 034 - pointer to TESObjectCELL (always zero so far)
         NiTPointerMap <void>	map;		// 038 - NiTPointerMap<int, class TESTerrainLODQuadRoot *>
         TESWorldSpace	* worldSpace048;	// 048
         UInt32			unk04C[(0x58 - 0x4C) >> 2];	// 04C
         TESClimate		* climate;			// 058
         UInt32			worldFlags;			// 05C
         UInt32			unk060;				// 060
         NiTPointerMap <void> map064;		// 064 - $NiTPointerMap@IPAV?$BSSimpleList@PAVTESObjectREFR@@@@@@
         Character		* character;		// 074
         void			* ptr078;			// 078
         TESWorldSpace	* parentWorldspace;	// 07C
         UInt32			unk080[(0xC0 - 0x80) >> 2];	// 080
         BSStringT			editorID;			// 0C0
         NiTPointerMap <void> map0C8;		// 0C8 - ?$NiTPointerMap@I_N@@
         UInt32			unk0D8[(0xE0 - 0xD8) >> 2];	// 0D8

         TESObjectCELL * LookupCell(SInt32 x, SInt32 y) const;
         bool IsOblivionWorld() const { return (worldFlags & kFlag_OblivionWorld) ? true : false; }
         bool CanFastTravel() const { return (worldFlags & kFlag_NoFastTravel) ? false : true; }
         void SetCanFastTravel(bool bCan) { if (bCan) worldFlags &= ~kFlag_NoFastTravel; else worldFlags |= kFlag_NoFastTravel; }

         MEMBER_FN_PREFIX(TESWorldSpace);
         DEFINE_MEMBER_FN(GetRootWorldspaceUnk84, Unk84*, 0x004EF1B0);
   };
};