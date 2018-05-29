#pragma once
#include "obse/NiObjects.h"
#include "obse/NiTypes.h"
#include "obse/Utilities.h"
#include "Core.h"

#define DIRECTINPUT_VERSION 0x0800
#define DIRECTSOUND_VERSION 0x0800
#include <dsound.h>

namespace RE {
   class TESGameSound { // sizeof == 0x58
      public:
         TESGameSound();
         ~TESGameSound();

         enum {
            kType_Voice = 1 << 2,
            kType_Footstep = 1 << 3,
         };

         UInt8			typeFlags;	// 00	for calculating volume based on OSSoundGlobals::foot/voice/effectVolume
                                 //      effectVolume used by default
         UInt8			unk01;
         UInt8			unk02[2];
         UInt32			unk04[2];	// 04
         UInt32			hashKey;	// 0C
         UInt32			unk10[4];	// 10
         float			x;			// 20
         float			y;			// 24
         float			z;			// 28
         UInt32			unk2C[4];	// 2C
         float			unk3C;		// 3C
         UInt32			unk40[3];	// 40
         const char*	name;		// 4C
         UInt32			unk50;		// 50
         UInt32			unk54;		// 54
         
         MEMBER_FN_PREFIX(TESGameSound);
         DEFINE_MEMBER_FN(Subroutine006B7260, bool, 0x006B7260);
         //
         DEFINE_MEMBER_FN(Subroutine006B7190, void, 0x006B7190, UInt32); // generally run if 006B7260 returns false
         DEFINE_MEMBER_FN(Subroutine006B7220, void, 0x006B7220); // generally run if 006B7260 returns true
         DEFINE_MEMBER_FN(Subroutine006B7240, void, 0x006B7240); // generally run if 006B7260 returns true
         DEFINE_MEMBER_FN(Subroutine006B73C0, void, 0x006B73C0);
         DEFINE_MEMBER_FN(Destructor,         void, 0x006B73E0);
   };
   class OSSoundGlobals { // sizeof == 0x328
      public:
         OSSoundGlobals();
         ~OSSoundGlobals();

         enum {
            kFlags_HasDSound = 1 << 0,
            kFlags_HasHardware3D = 1 << 2,
         };

         typedef NiTPointerMap<TESGameSound> TESGameSoundMap;
         typedef NiTPointerMap<NiAVObject>   NiAVObjectMap;

         UInt32					unk000;						// 000
         UInt32					unk004;						// 004
         IDirectSound8* dsoundInterface;			// 008
         IDirectSoundBuffer8* primaryBufferInterface;	// 00C
         DSCAPS					soundCaps;					// 010
         UInt32					unk070;						// 070
         UInt32					unk074;						// 074
         IDirectSound3DListener* listenerInterface;		// 078
         UInt32					unk07C;						// 07C
         float					PlayerPosX;					// 080
         float					PlayerPosY;					// 084
         float					PlayerPosZ;					// 088
         UInt32					unk08C[(0x0A4 - 0x08C) >> 2];	// 08C
         UInt8					unk0A4;						// 0A4
         UInt8					unk0A5;						// 0A5
         UInt8					unk0A6;						// 0A6
         UInt8					pad0A7;						// 0A7
         UInt32					unk0A8;						// 0A8
         UInt32					flags;						// 0AC - flags?
         UInt32					unk0B0;						// 0B0
         float					unk0B4;						// 0B4
         float					masterVolume;				// 0B8
         float					footVolume;					// 0BC
         float					voiceVolume;				// 0C0
         float					effectsVolume;				// 0C4
         UInt32					unk0C8;						// 0C8 - time
         UInt32					unk0CC;						// 0CC - time
         UInt32					unk0D0;						// 0D0 - time
         UInt32					unk0D4[(0x0DC - 0x0D4) >> 2];	// 0D4
         UInt32					unk0DC;						// 0DC
         UInt32					unk0E0[(0x2F0 - 0x0E0) >> 2];	// 0E0
         float					musicVolume;				// 2F0
         UInt32					unk2F4;						// 2F4
         float					musicVolume2;				// 2F8
         UInt32					unk2FC;						// 2FC
         TESGameSoundMap* gameSoundMap;				// 300
         NiAVObjectMap* niObjectMap;				// 304
         NiTPointerList<void>* soundMessageMap;			// 308 - AudioManager::SoundMessage *
         UInt32					unk30C[(0x320 - 0x30C) >> 2];	// 30C
         void* soundMessageList;			// 320
         UInt32					unk324;						// 324

                                                      
         MEMBER_FN_PREFIX(OSSoundGlobals);
         DEFINE_MEMBER_FN(Subroutine006ADE50, TESGameSound*, 0x006ADE50, const char* soundEditorID, UInt32, UInt32);  // used by LockPickMenu
         DEFINE_MEMBER_FN(PlaySound,          TESGameSound*, 0x006AE0A0, UInt32 soundRefID, UInt32 flags, bool arg2); // used by PlaySound command // flags: seen 0x101 (non-locational), 0x102 (locational)
   };
};