#pragma once
#include "Miscellaneous.h"
#include "obse/NiTypes.h"

namespace RE {
   class SaveGameFile; // subclass of BSFile

   class TESSaveLoadGame { // sizeof == 0x1C4
      //
      // Constructor: 0045B300; no args
      //
      // Class seems to handle all savedata; everything routes through it.
      //
      public:
         NiTMapBase<UInt32, UInt32>* unk00; // 00 // related to ChangeData
         UInt32 unk04 = 0;
         NiTMapBase<UInt32, UInt32>* unk08; // 08 // a map of BSSimpleList<int> instances?
         NiTMapBase<UInt32, UInt32>* unk0C; // 0C // related to ExteriorCellReferenceData
         NiTMapBase<UInt32, UInt32>* unk10; // 10 // BSSimpleList<ExteriorCellReferenceData>
         UInt32 unk14 = 0;
         UInt32 unk18 = 0; // bitmask
         UInt32 unk1C = 0;
         UInt32 unk20 = 0;
         UInt32 unk24 = 0;
         UInt32 unk28 = 0;
         UInt32 unk2C = 0;
         UInt32 unk30 = 0;
         UInt32 unk34 = 0;
         UInt32 unk38 = 0;
         UInt32 unk3C = 0;
         UInt32 unk40 = 0;
         UInt32 unk44;
         UInt32 unk48;
         UInt32 unk4C = 0;
         UInt32 unk50 = 0;
         NiTMapBase<UInt32, UInt32>* unk54; // 54
         NiTMapBase<UInt32, UInt32>* unk58; // 58 // same type as unk54
         NiTMapBase<UInt32, UInt32>* unk5C; // 5C // same type as unk54 and unk58
         NiTMapBase<UInt32, UInt32>* unk60; // 60 // same type as unk54, unk58, ...
         UInt32 unk64 = 0;
         UInt32 unk68 = 0;
         LinkedPointerList<SaveGameFile>* saveFiles = nullptr; // 6C
         UInt8  unk70 = 0;
         UInt8  unk71 = 0x7D;
         UInt16 unk72;
         NiTLargeArray<int>* unk74; // 74
         NiTLargeArray<int>* unk78; // 78
         UInt8  unk7C = 0x7D;
         UInt8  unk7D = 0;
         UInt16 unk7E;
         UInt32 unk80 = 0;
         UInt32 unk84 = 0;
         UInt32 unk88 = 0;
         UInt32 unk8C = 0;
         UInt32 unk90 = 0;
         UInt32 unk94 = 0;
         UInt32 unk98 = 0;
         UInt32 unk9C = 0;
         UInt32 unkA0 = 0;
         UInt32 unkA4 = 0;
         UInt8  unkA8 = 0;
         UInt8  unkA9 = 0;
         UInt8  unkAA = 0;
         UInt8  unkAB = 0;
         UInt32 unkAC = 0;
         UInt32 unk100[(0x1C0 - 0x100) / 4]; // 100
         UInt32 unk1C0 = 0; // 1C0

         MEMBER_FN_PREFIX(TESSaveLoadGame);
         DEFINE_MEMBER_FN(ClearSaveFileList,  void, 0x00459400); // deletes the list and the in-memory SaveGameFile instances (NOT the actual, real files); also calls 005AE430, which nulls LoadgameMenu::unk4C
         DEFINE_MEMBER_FN(DeleteSaveFile,     void, 0x00453480, SaveGameFile* target, UInt32 alwaysZero); // deletes the actual file on the hard drive; NorthernUI patches it to send to the Recycle Bin instead
         DEFINE_MEMBER_FN(UpdateSaveFileList, void, 0x0045D450); // calls ClearSaveFileList if necessary, and then rebuilds (saveFiles)
   };
};