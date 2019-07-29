#pragma once
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "ReverseEngineered/Systems/AI.h"

namespace RE {
   class MobileObject : public TESObjectREFR {
      public:
         enum {
            //	UInt8	processLevel;
            //		FF - none
            //		00 - high
            //		01 - medium high
            //		02 - medium low
            //		03 - low
         };

         MobileObject();
         ~MobileObject();

         virtual void	Unk_6A(void) = 0;	// 6A
         virtual void	Unk_6B(void) = 0;
         virtual void	Unk_6C(void) = 0;
         virtual void	Move(float arg0, NiVector3& offset, UInt32 movementFlags) = 0;
         virtual void	Jump(void) = 0;	// jump?
         virtual void	Unk_6F(bool) = 0;
         virtual void	Unk_70(void) = 0;	// 70
         virtual void	Unk_71(void) = 0;
         virtual bool	Unk_72() = 0;
         virtual void	SetPosition(NiVector3&) = 0; // 73
         virtual void	Unk_74(void) = 0;
         virtual void	Unk_75(void) = 0;
         virtual void	Unk_76(void) = 0;
         virtual void	Unk_77(void) = 0;
         virtual float	GetZRotation(void) = 0; // 78
         virtual void	Unk_79(void) = 0;
         virtual void	SetZRotation(float) = 0; // 7A
         virtual void	Unk_7B(void) = 0;
         virtual void	Unk_7C(void) = 0;
         virtual float	GetJumpScale() = 0;
         virtual bool	Unk_7E() = 0; // returns this->deadState == 1
         virtual void	Unk_7F(void) = 0;
         virtual void	Unk_80(void) = 0;	// 80
         virtual SInt32	GetFame(void) = 0;  //OR definition

         BaseProcess* process;			// 058

         MEMBER_FN_PREFIX(MobileObject);
         DEFINE_MEMBER_FN(ClearAllMovementFlags,    void,   0x005E05D0);
         DEFINE_MEMBER_FN(GetCharacterProxy,        bhkCharacterProxy*, 0x0065A2C0);
         DEFINE_MEMBER_FN(GetCurrentAction,         SInt32, 0x005E0EE0);
         DEFINE_MEMBER_FN(IsSwimming,               bool,   0x005E0530);
         DEFINE_MEMBER_FN(ModifyPitch,              void,   0x0065ABC0, float radians);
         DEFINE_MEMBER_FN(ModifyYaw,                void,   0x00659B90, float radians);
         DEFINE_MEMBER_FN(MovementFlagsIncludeWASD, bool,   0x005E05B0);
         DEFINE_MEMBER_FN(SetPitch,                 void,   0x0065A650, float pitch); // unlike TESObjectREFR, enforces pitch constraints
   };
};