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
         virtual void	Move(float arg0, float* pos, UInt32 arg2) = 0;
         virtual void	Jump(void) = 0;	// jump?
         virtual void	Unk_6F(void) = 0;
         virtual void	Unk_70(void) = 0;	// 70
         virtual void	Unk_71(void) = 0;
         virtual void	Unk_72(void) = 0;
         virtual void	Unk_73(void) = 0;
         virtual void	Unk_74(void) = 0;
         virtual void	Unk_75(void) = 0;
         virtual void	Unk_76(void) = 0;
         virtual void	Unk_77(void) = 0;
         virtual float	GetZRotation(void) = 0;
         virtual void	Unk_79(void) = 0;
         virtual void	SetZRotation(float) = 0;
         virtual void	Unk_7B(void) = 0;
         virtual void	Unk_7C(void) = 0;
         virtual float	GetJumpScale(void) = 0;
         virtual void	Unk_7E(void) = 0;
         virtual void	Unk_7F(void) = 0;
         virtual void	Unk_80(void) = 0;	// 80
         virtual SInt32	GetFame(void) = 0;  //OR definition

         BaseProcess* process;			// 058
   };
};