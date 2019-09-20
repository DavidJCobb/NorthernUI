#pragma once
#include "obse/GameProcess.h"

class bhkCharacterController;
class NiBillboardNode;
namespace RE {
   enum DetectionState : UInt32 {
      kDetectionState_Lost = 0,
      kDetectionState_Unseen,
      kDetectionState_Noticed,
      kDetectionState_Seen,

      kDetectionState_Max
   };
   enum ProcessLevel : UInt32 {
      kProcessLevel_High = 0,
      kProcessLevel_MiddleHigh = 1, // assumed
      kProcessLevel_MiddleLow  = 2, // assumed
      kProcessLevel_Low  = 3,
   };

   class Actor;
   class TESObjectREFR;
   class BaseProcess {
      public:
         enum {
            //	RefIDIdx	combatController;
         };
         enum {
            kAction_None                   = -1,
            kAction_EquipWeapon            = 0,
            kAction_UnequipWeapon          = 1,
            kAction_Attack                 = 2,
            kAction_AttackFollowThrough    = 3,
            kAction_AttackBow              = 4,
            kAction_AttackBowArrowAttached = 5,
            kAction_Block   = 6,
            kAction_Recoil  = 7,
            kAction_Stagger = 8,
            kAction_Dodge   = 9,
            kAction_LowerBodyAnim   = 10,
            kAction_SpecialIdle     = 11,
            kAction_ScriptAnimation = 12,
         };
         enum {
            kMovement_Forward   = 0x0001,
            kMovement_Backward  = 0x0002,
            kMovement_Left      = 0x0004,
            kMovement_Right     = 0x0008,
            kMovement_TurnLeft  = 0x0010,
            kMovement_TurnRight = 0x0020,
            //
            kMovement_Walk  = 0x0100,
            kMovement_Run   = 0x0200,
            kMovement_Sneak = 0x0400,	// overridden by kMovementFlag_Swimming
            kMovement_Swim  = 0x0800,
            kMovement_Jump  = 0x1000, //Jump and above appear not to be used.
            kMovement_Fly   = 0x2000,
            kMovement_Fall  = 0x4000,
            kMovement_Slide = 0x8000,
            //
            // Utility:
            //
            kMovement_AnyWASD    = 0x000F,
            kMovement_AnyFwdBack = 0x0003,
            kMovement_AnyStrafe  = 0x000C,
            kMovement_Any        = 0x003F,
         };

         BaseProcess();
         ~BaseProcess();

         virtual void	Destructor(void);
         virtual void	Unk_01(void) = 0;
         virtual ProcessLevel	GetProcessLevel(void) = 0;	// 0 - high, 3 - low
         virtual void	Unk_03(Actor* Act, UInt32 Arg1) = 0;  //Or Definition
         virtual void	ManagePackProcedure(Actor* Act) = 0;   //Or definition
         virtual void	Unk_05(UInt32 arg0, UInt32 arg1) = 0;
         virtual bool	Unk_06(UInt32 arg0, UInt32 arg1) = 0;
         virtual void	SetCurHour(float Hour) = 0;
         virtual void	Unk_08(void) = 0;
         virtual bool	Unk_09(void) = 0;
         virtual float	GetCurHour(void) = 0;
         virtual UInt32	GetCurPackedDate(void) = 0;
         virtual void	SetCurPackedDate(UInt32 arg0) = 0;
         virtual UInt32	GetCurDay(void) = 0;
         virtual UInt32	GetCurMonth(void) = 0;
         virtual UInt32	GetCurYear(void) = 0;
         virtual void	Unk_10(UInt32 arg0) = 0;
         virtual UInt32	Unk_11(void) = 0;
         virtual void	Unk_12(UInt32 arg0) = 0;
         virtual void	Unk_13(void) = 0;
         virtual UInt32	Unk_14(void) = 0;
         virtual void	Unk_15(UInt32 arg0) = 0;
         virtual void	Unk_16(UInt32 arg0, UInt32 arg1) = 0;
         virtual void	Unk_17(void) = 0;
         virtual void	UpdateHealthBar(Actor*) = 0; // for INI:Gameplay:bHealthBarShowing
         virtual bool	Unk_19(void) = 0;
         virtual void	Unk_1A(UInt32 arg0) = 0;
         virtual bool	Unk_1B(UInt32 arg0, UInt32 arg1) = 0;
         virtual void	Unk_1C(UInt32 arg0, UInt32 arg1, UInt32 arg2) = 0;
         virtual UInt32	Unk_1D(UInt32 arg0) = 0;
         virtual UInt32	Unk_1E(UInt32 arg0) = 0;
         virtual float	Unk_1F(UInt32 arg0) = 0;
         virtual void	Unk_20(UInt32 arg0, UInt32 arg1) = 0;
         virtual void	Unk_21(UInt32 arg0, UInt32 arg1, UInt32 arg2) = 0;
         virtual bool	Unk_22(void) = 0;
         virtual void	Unk_23(UInt32 arg0) = 0;
         virtual void	Unk_24(Actor* subject) = 0;
         virtual void	Unk_25(void) = 0;
         virtual void	UpdateUnk088(void) = 0;
         virtual float	GetUnk088(void) = 0;
         virtual void	SetUnk088(float arg0) = 0;
         virtual void	Unk_29(void) = 0;

         // returns HighProcess::DetectionList::Data* associated with detectedActor. detectionState is one of kDetectionState_XXX enum
         // ###TODO: move some of this stuff out of HighProcess class def so we can forward declare it
         virtual void*  SetDetected(Actor* detectedActor, UInt8 detectionState, bool hasLOS, SInt32 detectionLevel) = 0;
         virtual bool	Unk_2B(void) = 0;
         virtual void	Unk_2C(UInt32 arg0) = 0;
         virtual void	Unk_2D(void) = 0;	// added in 1.2 patch
         virtual void	Unk_2E(UInt32 arg0) = 0;
         virtual void	Unk_2F(UInt32 arg0) = 0;
         virtual bool	Unk_30(void) = 0;
         virtual void	Unk_31(UInt32 arg0) = 0;
         virtual bool	Unk_32(void) = 0;
         virtual TESObjectREFR *	GetUnk02C(void) = 0; // 33
         virtual void	SetUnk02C(TESObjectREFR * obj) = 0; // 34
         virtual UInt32	Unk_35(void) = 0;
         virtual void	Unk_36(UInt32 arg0) = 0;
         virtual void	Unk_37(UInt32 arg0) = 0;
         virtual UInt32	Unk_38(void) = 0;
         virtual void	Unk_39(UInt32 arg0) = 0;
         virtual NiObject*	GetFaceAnimationData(Actor* subject) = 0; // 3A
         virtual ExtraContainerChanges::EntryData*  GetEquippedWeaponData(bool mustBeInHands) = 0; // 3B
         virtual void*  Unk_3C(UInt32 arg0) = 0;
         virtual ExtraContainerChanges::EntryData* GetEquippedAmmoData(bool mustBeInHands) = 0;
         virtual UInt32	Unk_3E(UInt32 arg0) = 0;
         virtual UInt32 Unk_3F() = 0;
         virtual void	Unk_40(UInt32 arg0) = 0;
         virtual bool	Unk_41(UInt32 arg0, UInt32 arg1) = 0;
         virtual bool	Unk_42(UInt32 arg0) = 0;
         virtual bool	Unk_43(UInt32 arg0) = 0;
         virtual bool	Unk_44(UInt32 arg0) = 0;
         virtual bool	Unk_45(UInt32 arg0) = 0;
         virtual UInt32	Unk_46(UInt32 arg0) = 0;
         virtual UInt32	Unk_47(UInt32 arg0) = 0;
         virtual UInt32	Unk_48(UInt32 arg0) = 0;
         virtual UInt32	Unk_49(UInt32 arg0) = 0;
         virtual UInt32	Unk_4A(UInt32 arg0) = 0;
         virtual void*  Unk_4B(UInt32 arg0) = 0;
         virtual UInt32	Unk_4C(UInt32 arg0) = 0;
         virtual UInt32	Unk_4D(UInt32 arg0) = 0;
         virtual bool	Unk_4E() = 0;
         virtual bool	Unk_4F(void) = 0;
         virtual UInt8	GetUnk084(void) = 0;
         virtual void	SetUnk084(UInt8 arg0) = 0;
         virtual bool	GetUnk01E(void) = 0;
         virtual void	SetUnk01E(UInt32 arg0) = 0;
         virtual void	Unk_54(UInt32 arg0, UInt32 arg1, UInt32 arg2, UInt32 arg3) = 0;
         virtual void	SetUnk024(UInt32 arg) = 0;
         virtual void	Unk_56(void) = 0; // related to AI packages, sleep, and fChase3rdPersonZUnitsPerSecond
         virtual void	Unk_57(void) = 0;
         virtual void	Unk_58(float) = 0; // called by Unk_56 with fChase3rdPersonZUnitsPerSecond as the argument
         virtual void	GetUnk028(float arg) = 0;
         virtual float	SetUnk028(void) = 0;
         virtual void	Unk_5B(void) = 0;
         virtual void	Unk_5C(void) = 0;
         virtual TESPackage*	GetCurrentPackage() = 0;
         virtual void					SetCurrentPackage(TESPackage* Package) = 0;
         virtual void					SetCurrentPackProcedure(TESPackage::eProcedure PackProcedure) = 0; //OR def Note OR definiton say this return a eProcedure
         virtual TESPackage::eProcedure	GetCurrentPackProcedure(void) = 0;  //OR def
         virtual UInt32				GetCurrentPackageFormFlags() = 0; // 61
         virtual void					Unk_62(UInt32 arg0) = 0;			// marks ScriptEventList::kEvent_OnPackageDone
         virtual NiPointer<bhkCharacterProxy>* GetCharProxy(NiPointer<bhkCharacterProxy>& characterProxy) = 0; // 63 // increfs and returns the proxy (or sets to NULL)
         virtual void	Unk_64(void * obj) = 0;
         virtual void	Unk_65(Actor*) = 0;
         virtual void	Unk_66(void) = 0;
         virtual void	Unk_67(void) = 0;
         virtual void	Unk_68(void) = 0;
         virtual void	SayTopic(Character* speaker, TESTopic* topic, bool bForceSubtitles, UInt32 unk3, UInt32 unk4) = 0; // unks: 0, 0
         virtual void	Unk_6A(void) = 0;
         virtual void	Unk_6B(void) = 0;
         virtual void	Unk_6C(void) = 0;
         virtual void	Unk_6D(void) = 0;
         virtual void	Unk_6E(void) = 0;
         virtual void	Unk_6F(void) = 0;
         virtual void	Unk_70(void) = 0;
         virtual void	Unk_71(void) = 0;
         virtual bool	GetLOS(UInt32 arg0, TESObjectREFR* target) = 0;	// arg0 unused
         virtual bool	Unk_73() = 0; // possibly "is in dialogue"
         virtual void	Unk_74(void) = 0;
         virtual void	Unk_75(void) = 0;
         virtual void	Unk_76(void) = 0;
         virtual void	Unk_77(void) = 0;
         virtual void	Unk_78(void) = 0;
         virtual void	Unk_79(void) = 0;
         virtual void	Unk_7A(void) = 0;
         virtual void	Unk_7B(void) = 0;
         virtual void	Unk_7C(void) = 0;
         virtual void	Unk_7D(void) = 0;
         virtual void	Unk_7E(void) = 0;
         virtual void	Unk_7F(void) = 0;
         virtual void	Unk_80(void) = 0;
         virtual void	Unk_81(void) = 0;
         virtual void	Unk_82(void) = 0;
         virtual void	Unk_83(void) = 0;
         virtual UInt8	GetIsAlerted(void) = 0;  //OR definition
         virtual void	SetIsAlerted(UInt8 IsAlerted) = 0; //OR definition
         virtual void	Unk_86(void) = 0;
         virtual void	Unk_87(void) = 0;
         virtual void	Unk_88(void) = 0;
         virtual void	Unk_89(void) = 0;
         virtual void	Unk_8A(void) = 0;
         virtual void	Unk_8B(void) = 0;
         virtual void	Unk_8C(void) = 0;
         virtual void	Unk_8D(void) = 0;
         virtual void	Unk_8E(void) = 0;
         virtual void	Unk_8F(void) = 0;
         virtual void	Unk_90(void) = 0;
         virtual void	Unk_91(void) = 0;
         virtual UInt8	GetUnk020(void) = 0;
         virtual void	SetUnk020(UInt8 arg) = 0;
         virtual void	Unk_94(void) = 0;
         virtual void	Unk_95(void) = 0;
         virtual void	Unk_96(void) = 0;
         virtual void	Unk_97(void) = 0;
         virtual void	Unk_98(void) = 0;
         virtual void	Unk_99(void) = 0;
         virtual void	Unk_9A(void) = 0;
         virtual void	Unk_9B(void) = 0;
         virtual void	Unk_9C(void) = 0;
         virtual void	Unk_9D(void) = 0;
         virtual void	Unk_9E(void) = 0;
         virtual void	Unk_9F(void) = 0;
         virtual void	Unk_A0(void) = 0;
         virtual SInt32 Unk_A1(UInt32) = 0; // A1
         virtual void	Unk_A2(void) = 0;
         virtual void	Unk_A3(void) = 0;
         virtual void	Unk_A4(void) = 0;
         virtual void	Unk_A5(void) = 0;
         virtual void	Unk_A6(void) = 0;
         virtual void	Unk_A7(void) = 0;
         virtual void	Unk_A8(void) = 0;
         virtual void	Unk_A9(void) = 0;
         virtual void	Unk_AA(void) = 0;
         virtual MagicItem* GetQueuedMagicItem(void) = 0;	// returns MiddleHighProcess+144
         virtual void	Unk_AC(void) = 0;
         virtual void	Unk_AD(void) = 0;
         virtual UInt8  Unk_AE() = 0; // returns byte unk14C
         virtual void	Unk_AF(void) = 0;
         virtual UInt32	GetMovementFlags() = 0; // B0
         virtual void	ModifyMovementFlags(UInt32 bits, bool clearOrSet) = 0; // B1
         virtual void	SetMovementFlags(UInt16) = 0; // B2
         virtual void	Move(Actor* subject, NiVector3 moveOffset_onlyUsedIfCollisionDisabled) = 0; // B3 // carries out WASD movement
         virtual SInt32	GetCurrentAction() = 0; // B4
         virtual BSAnimGroupSequence* GetCurrentActionAnimation() = 0; // B5
         virtual void	SetCurrentAction(SInt32 action, BSAnimGroupSequence*) = 0; // B6

         // action is one of kAction_XXX. Returns action, return value probably unused.
         virtual bool	Unk_B7() = 0; // tests anim group type
         virtual bool	Unk_B8() = 0;
         virtual UInt8  GetKnockedState() = 0; // B9
         virtual void   SetKnockedState(UInt8) = 0; // BA
         virtual void	Unk_BB(UInt32) = 0; // BB // no-op on HighProcess
         virtual void	Unk_BC(UInt32, UInt32, UInt32, UInt32, UInt32) = 0; // BC
         // arg3 is a multiplier, arg4 appears to be base force to apply
         virtual UInt32 Unk_BD(Actor*) = 0;
         virtual void	Unk_BE(Actor*) = 0; // handles knockdown face anim and state
         virtual UInt8	GetCombatMode(void) = 0;
         virtual UInt8	SetCombatMode(UInt8 CombatMode) = 0;
         virtual bool   GetWeaponOut() = 0; // C1
         virtual UInt8	SetWeaponOut(UInt8 WeaponOut) = 0;

         virtual void	SetUnk20C(NiVector3) = 0; // C3
         virtual void*	Unk_C4(void) = 0;	// returns some pointer
         virtual void	Unk_C5(void) = 0;
         virtual void	Unk_C6(void) = 0;
         virtual void	Unk_C7(void) = 0;
         virtual void	Unk_C8(void) = 0;
         virtual void	Unk_C9(void) = 0;
         virtual void	Unk_CA(void) = 0;
         virtual void	Unk_CB(void) = 0;
         virtual void	Unk_CC(void) = 0;
         virtual void	Unk_CD(void) = 0;
         virtual void	Unk_CE(void) = 0;
         virtual void	Unk_CF(void) = 0;
         virtual void	Unk_D0(void) = 0;
         virtual void	Unk_D1(void) = 0;
         virtual void	Unk_D2(void) = 0;
         virtual void	Unk_D3(void) = 0;
         virtual void	Unk_D4(void) = 0;
         virtual void	Unk_D5(void) = 0;
         virtual void	Unk_D6(void) = 0;
         virtual void	Unk_D7(void) = 0;
         virtual void	Unk_D8(void) = 0;
         virtual void	Unk_D9(void) = 0;
         virtual void	Unk_DA(void) = 0;
         virtual UInt8	GetSitSleepState() = 0; // DB
         virtual void	SetCurrentFurniture(Actor* actor, UInt32 sitSleepState, TESObjectREFR* furniture, UInt32) = 0; // DC
         virtual void	Unk_DD(void) = 0;
         virtual TESObjectREFR* GetFurniture() = 0;
         virtual void	Unk_DF(void) = 0;
         virtual void	Unk_E0(void) = 0;
         virtual UInt32	GetUnk0D4(void) = 0;
         virtual void	Unk_E2(void) = 0;
         virtual void	Unk_E3(void) = 0;
         virtual UInt8	GetUnk01C(void) = 0;
         virtual void	SetUnk01C(UInt8 arg) = 0;
         virtual UInt8	GetUnk180(void) = 0;
         virtual void	SetUnk180(UInt8 arg) = 0;
         virtual void	Unk_E8(void) = 0;
         virtual void	Unk_E9(void) = 0;
         virtual void	Unk_EA(void) = 0;
         virtual float	GetLightAmount(Actor * actor, UInt32 unk1) = 0;
         virtual void	Unk_EC(void) = 0;
         virtual void*  GetDetectionState(Actor* target) = 0; // returns HighProcess::DetectionList::Data*
         virtual void	Unk_EE(void) = 0;
         virtual void	Unk_EF(void) = 0;
         virtual void	Unk_F0(void) = 0;
         virtual void	Unk_F1(void) = 0;
         virtual void	Unk_F2(void) = 0;
         virtual void	Unk_F3(void) = 0;
         virtual void	Unk_F4(void) = 0;
         virtual void	Unk_F5(void) = 0;
         virtual void	Unk_F6(void) = 0;
         virtual void	Unk_F7(void) = 0;
         virtual void	Unk_F8(void) = 0;
         virtual void	Unk_F9(void) = 0;
         virtual void	Unk_FA(void) = 0;
         virtual void	Unk_FB(void) = 0;
         virtual void	Unk_FC(void) = 0;
         virtual void	Unk_FD(void) = 0;
         virtual void	Unk_FE(void) = 0;
         virtual void	Unk_FF(void) = 0;
         virtual void	Unk_100(void) = 0;
         virtual void	Unk_101(void) = 0;
         virtual void	Unk_102(void) = 0;
         virtual PathLow* CreatePath(void) = 0;
         virtual PathLow* GetCurrentPath(void) = 0;
         virtual void	Unk_105(void) = 0;
         virtual void	Unk_106(void) = 0;
         virtual void	Unk_107(void) = 0;
         virtual void	Unk_108(void) = 0;
         virtual void	Unk_109(void) = 0;
         virtual void	Unk_10A(void) = 0;
         virtual void	Unk_10B(void) = 0;
         virtual void	Unk_10C(void) = 0;
         virtual void	Unk_10D(void) = 0;
         virtual void	Unk_10E(void) = 0;
         virtual void	Unk_10F(void) = 0;
         virtual void	Unk_110(void) = 0;
         virtual void	Unk_111(void) = 0;
         virtual void	Unk_112(void) = 0;
         virtual void	Unk_113(void) = 0;
         virtual void	Unk_114(void) = 0;
         virtual void	Unk_115(void) = 0;
         virtual void	Unk_116(void) = 0;
         virtual void	Unk_117(void) = 0;
         virtual void	Unk_118(void) = 0;
         virtual void	Unk_119(void) = 0;
         virtual void	Unk_11A(void) = 0;
         virtual void*  Unk_11B() = 0;
         virtual void	Unk_11C(void) = 0;
         virtual void	Unk_11D(void) = 0;
         virtual void	Unk_11E(void) = 0;
         virtual UInt32 Unk_11F() = 0; // returns enum? // returns HighProcess::unk2BC
         virtual void	Unk_120(void) = 0;
         virtual void	Unk_121(Actor*) = 0;
         virtual void	Unk_122(void) = 0;
         virtual void	Unk_123(void) = 0;
         virtual void	Unk_124(void) = 0;
         virtual void	Unk_125(void) = 0;
         virtual void	Unk_126(void) = 0;
         virtual void	Unk_127() = 0;
         virtual void	Unk_128(void) = 0;
         virtual void	Unk_129(void) = 0;
         virtual void	Unk_12A(void) = 0;
         virtual void	Unk_12B(void) = 0;
         virtual void	Unk_12C() = 0;
         virtual void	Unk_12D(void) = 0;
         virtual void	Unk_12E(void) = 0;
         virtual void	Unk_12F(void) = 0;
         virtual void	Unk_130(void) = 0;
         virtual void	Unk_131(void) = 0;
         virtual void	Unk_132(void) = 0;

         // action type is one of HighProcess::kActionType_XXX
         virtual TESObjectREFR* GetActionTarget(UInt32 actionType) = 0;
         virtual void	Unk_134(void) = 0;
         virtual const char*	Unk_135(void) = 0;
         virtual void	Unk_136(void) = 0;
         virtual void	Unk_137(void) = 0;
         virtual void	Unk_138(void) = 0;
         virtual void	Unk_139(void) = 0;
         virtual void	Unk_13A(void) = 0;
         virtual void	Unk_13B(void) = 0;
         virtual float	GetUnk08C(void) = 0;
         virtual void	SetUnk08C(float arg) = 0;
         virtual void	Unk_13E(void) = 0;
         virtual void	Unk_13F(void) = 0;
         virtual void	Unk_140(void) = 0;
         virtual void	Unk_141(void) = 0;

         //	void					** _vtbl;				// 000
         TESPackage::eProcedure	editorPackProcedure;	// 004
         TESPackage				* editorPackage;		// 008
   };
   class LowProcess : public BaseProcess { // sizeof == 0x90
      public:
         LowProcess();
         ~LowProcess();

         virtual void	Unk_141(void) = 0;
         virtual void	Unk_142(void) = 0;
         virtual void	Unk_143(void) = 0;
         virtual void	Unk_144(void) = 0;
         virtual void	Alarm(Actor* Act) = 0;
         virtual void	Unk_146(void) = 0;
         virtual void	Unk_147(void) = 0;
         virtual void	Unk_148(void) = 0;
         virtual void	Unk_149(void) = 0;
         virtual void	Unk_14A(void) = 0;
         virtual void	Unk_14B(void) = 0;
         virtual void	Unk_14C(void) = 0;
         virtual void	Unk_14D(void) = 0;
         virtual void	Unk_14E(void) = 0;
         virtual void	Unk_14F(void) = 0;
         virtual void	Unk_150(void) = 0;
         virtual void	Unk_151(void) = 0;
         virtual void	Unk_152(void) = 0;
         virtual void	Unk_153(void) = 0;
         virtual void	Unk_154(void) = 0;
         virtual void	Unk_155(void) = 0;
         virtual void	Unk_156(void) = 0;
         virtual UInt8	MountHorse(Actor* Act) = 0;				// 560
         virtual UInt8	DismountHorse(Actor* Act) = 0;			// 564
         virtual void	Unk_159(void) = 0;

         float	unk00C;				// 00C - initialized to -1
         float	unk010;				// 010
         float	curHour;			// 014 - initialized to -1
         UInt32	curPackedDate;		// 018 - (year << 13) | (month << 9) | day
         UInt8	unk01C;				// 01C
         UInt8	unk01D;				// 01D
         UInt8	unk01E;				// 01E
         UInt8	IsAlerted;			// 01F
         UInt8	unk020;				// 020
         UInt8	pad021[3];			// 021
         TESForm * usedItem;			// 024 for idles like reading book, making potions, etc
         float	unk028;				// 028
         Actor*	Follow;				// 02C
         TESObjectREFR   * unk030;	// 030 seen XMarkerHeading refs here
         PathLow	* pathing;			// 034
         UInt32	unk038;				// 038
         UInt32	unk03C;				// 03C
         UInt32	unk040;				// 040
         UInt32	unk044;				// 044
         UInt32	unk048;				// 048
         UInt32	unk04C;				// 04C
         UInt32	unk050;				// 050
         UInt32	unk054;				// 054
         UInt32	unk058;				// 058
         UInt32	unk05C;				// 05C
         UInt32	unk060;				// 060
         UInt32	unk064;				// 064
         UInt32	unk068;				// 068
         UInt32	unk06C;				// 06C
         ActorValues	avDamageModifiers;	// 070
         UInt8	unk084;				// 084
         UInt8	pad085[3];			// 085
         float	unk088;				// 088 - counter in seconds
         float	unk08C;				// 08C
   };
   static_assert(sizeof(LowProcess) == 0x90, "RE::LowProcess is the wrong size!");

   class MiddleLowProcess : public LowProcess { // sizeof == 0xA8
      public:
         MiddleLowProcess();
         ~MiddleLowProcess();

         virtual void	Unk_15A(void) = 0;

         UInt32			unk090;				// 090
         ActorValues		maxAVModifiers;		// 094
   };
   static_assert(sizeof(MiddleLowProcess) == 0xA8, "RE::MiddleLowProcess is the wrong size!");

   class MiddleHighProcess : public MiddleLowProcess { // sizeof == 0x18C
      public:
         MiddleHighProcess();
         ~MiddleHighProcess();

         virtual void	Unk_15B(void) = 0;
         virtual void	Unk_15C(void) = 0;
         virtual void	Unk_15D(void) = 0;
         virtual void	Unk_15E(void) = 0;
         virtual void	Unk_15F(void) = 0;
         virtual void	Dialogue(Actor* Act) = 0;
         virtual void	RemoveWornItems(Actor* Act, UInt8 Arg2, int Arg3) = 0;
         virtual void	Travel(Actor* Act, UInt8 Arg2, float Arg3, int Arg4 = 0) = 0;
         virtual void	Unk_163(void) = 0;
         virtual void	Unk_164(void) = 0;

         // 10
         struct Unk128 {
            UInt32	unk0;	// 0
            UInt32	unk4;	// 4
            UInt32	unk8;	// 8
            UInt16	unkC;	// C
            UInt8	unkE;	// E
            UInt8	padF;	// F
         };

         // 8
         struct EffectListNode {
            ActiveEffect	* effect;	// 0
            EffectListNode	* next;		// 4
         };

         UInt32				unk0A8;		// 0A8
         UInt32				unk0AC;		// 0AC
         UInt32				unk0B0;		// 0B0
         UInt32				unk0B4;		// 0B4
         UInt32				unk0B8;		// 0B8
         float				unk0BC;		// 0BC
         TESPackage			* currentPackage;	// 0C0 if null, editorPackage applies
         UInt32				unk0C4;		// 0C4
         UInt8				unk0C8;		// 0C8
         UInt8				pad0C9[3];	// 0C9
         TESPackage::eProcedure	currentPackProcedure;		// 0CC
         UInt8				unk0D0;		// 0D0
         UInt8				pad0D0[15];	// 0D1 - never initialized
         UInt32				unk0E0;		// 0E0
         ExtraContainerChanges::EntryData* equippedWeaponData;	// 0E4
         ExtraContainerChanges::EntryData* equippedLightData;	// 0E8
         ExtraContainerChanges::EntryData* equippedAmmoData;		// 0EC
         ExtraContainerChanges::EntryData* equippedShieldData;   // 0F0
         UInt8				unk0F4;		// 0F4
         UInt8				unk0F5;		// 0F5
         UInt8				pad0F6[2];	// 0F6
         float				unk0F8;		// 0F8
         UInt32				unk0FC;		// 0FC
         UInt32				unk100;		// 100
         UInt32				unk104;		// 104
         UInt32				unk108;		// 108
         UInt32				unk10C;		// 10C
         UInt32				unk110;		// 110
         UInt8				unk114;		// 114
         UInt8				unk115;		// 115
         UInt8				pad116[2];	// 116
         bhkCharacterProxy* charProxy;	// 118 - seen bhkCharacterProxy
         SInt8				knockedState;	// 11C  //TODO maybe unsigned??
         UInt8				sitSleepState; // 11D
         UInt8				pad11E;		// 11E
         UInt8				pad11F;		// 11F
         TESObjectREFR*		furniture;	// 120
         UInt8				unk124;		// 124 - init'd to 0x7F
         Unk128				unk128;		// 128
         UInt16				unk138;		// 138
         UInt8				pad13A[2];	// 13A
         UInt32				unk13C;		// 13C
         UInt32				unk140;		// 140
         MagicItem			* queuedMagicItem;	// 144 set before calling sub_69AF30 after Addspell cmd, unset upon return
         UInt32				unk148;		// 148
         UInt8				unk14C;		// 14C looks like true if casting, or possibly a casting state
         UInt8				pad14D[3];	// 14D
         UInt32				unk150;		// 150
         float				actorAlpha;	// 154 valid values 0-1
         float				unk158;		// 158
         NiExtraData			* unk15C;	// 15C seen BSFaceGenAnimationData*, reset when modifying character face // getter is BaseProcess::GetFaceAnimationData
         UInt8				unk160;		// 160
         UInt8				unk161;		// 161
         UInt8				pad162[2];	// 162
         UInt32				unk164;		// 164
         UInt8				unk168;		// 168
         UInt8				unk169;		// 169
         UInt8				unk16A;		// 16A
         UInt8				unk16B;		// 16B
         UInt8				unk16C;		// 16C
         UInt8				unk16D;		// 16D
         UInt8				pad16E[2];	// 16E
         UInt32				unk170;		// 170
         EffectListNode		* effectList;	// 174
         UInt32				unk178;		// 178
         ActorAnimData		* animData;	// 17C
         UInt8				unk180;		// 180
         UInt8				pad181[3];	// 181
         NiObject			* unk184;	// 184 - seen BSShaderPPLightingProperty
         BSBound				* boundingBox;	// 188

         bhkCharacterController* GetCharacterController();
   };
   static_assert(sizeof(MiddleHighProcess) == 0x18C, "RE::MiddleHighProcess is the wrong size!");

   class HighProcess : public MiddleHighProcess { // sizeof == 0x2EC // Oblivion Reloaded 7.0+ extends this to 0x300
      public:
         HighProcess();
         ~HighProcess();

         enum {
            kActionType_Default = 0,
            kActionType_Action,
            kActionType_Script,
            kActionType_Combat,
            kActionType_Dialog,

            kActionType_Max
         };
         enum {
            kDetectionState_Lost = 0,
            kDetectionState_Unseen,
            kDetectionState_Noticed,
            kDetectionState_Seen,

            kDetectionState_Max
         };

         struct DetectionList { // sizeof == 0x8
            struct Data {
               Actor			* actor;
               UInt8			detectionState;
               UInt8			pad04[3];
               UInt8			hasLOS;
               UInt8			pad08[3];
               SInt32			detectionLevel;
            };

            Data* data;
            DetectionList* next;

            Data* Info() const { return data; }
            DetectionList* Next() const { return next; }
         };
         typedef Visitor<DetectionList, DetectionList::Data> DetectionListVisitor;

         // this appears to be a common linked list class
         // 4
         struct Node190 {
            void	* data;
            Node190	* next;
         };

         // 10
         struct Unk1BC {
            UInt32	unk0;
            UInt32	unk4;
            UInt32	unk8;
            UInt32	unkC;
         };

         DetectionList	* detectionList;	// 18C
         Node190	unk190;		// 190
         UInt32	unk198;		// 198
         float	unk19C;		// 19C // initialized to GMST:fIdleChatterCommentTimer
         UInt8	unk1A0;		// 1A0
         UInt8	pad1A1[3];	// 1A1
         UInt32	unk1A4;		// 1A4
         UInt32	unk1A8;		// 1A8
         UInt32	unk1AC;		// 1AC
         UInt32	unk1B0;		// 1B0 - uninitialized
         UInt32	unk1B4;		// 1B4 - uninitialized
         UInt32	unk1B8;		// 1B8
         Unk1BC	unk1BC;		// 1BC
         UInt32	unk1CC;		// 1CC - uninitialized
         UInt8	unk1D0;		// 1D0
         UInt8	unk1D1;		// 1D1
         UInt32	unk1D4;		// 1D4
         UInt32	unk1D8;		// 1D8
         UInt32	unk1DC;		// 1DC
         UInt32	unk1E0;		// 1E0
         UInt8	unk1E4;		// 1E4
         UInt8	pad1E5[3];	// 1E5
         UInt32	unk1E8;		// 1E8
         UInt32	unk1EC;		// 1EC
         UInt32	unk1F0;		// 1F0
         SInt16	currentAction;	// 1F4 - related to 1F8. returned by vtbl +2D0
         UInt8	pad1F6[2];	// 1F6
         UInt32	unk1F8;		// 1F8 - related to 1F4. Sometimes a BSAnimGroupSequence*
         UInt16	movementFlags;	// 1FC - returned by vtbl + 2C0
         UInt8	pad1FE[2];	// 1FE
         UInt32	unk200;		// 200
         float	unk204;		// 204
         UInt16	unk208;		// 208
         UInt8	pad20A[2];	// 20A
         NiVector3 unk20C; // 20C
         UInt32	unk218;		// 218
         float	unk21C;		// 21C
         UInt32	unk220[2];	// 220
         UInt8	unk228;		// 228
         UInt8	pad229[3];	// 229
         float	unk22C;		// 22C
         float	unk230;		// 230 - initialized to ((rand() % 5000) * .001) + 10
         UInt32	unk234;		// 234 - not initialized
         float	swimBreath;		// 238 - initialized to 0x41A00000
         UInt8	unk23C;		// 23C
         UInt8	unk23D[3];	// 23D
         UInt32	unk240;		// 240
         UInt8	unk244;		// 244
         UInt8	unk245[3];	// 245
         float	unk248;		// 248 // camera distance?
         UInt8	unk24C;		// 24C
         UInt8	pad24D[3];	// 24D
         UInt32	unk250;		// 250
         UInt32	unk254;		// 254
         UInt32	unk258;		// 258
         UInt8	unk25C;		// 25C
         UInt8	unk25D;		// 25D
         UInt8	pad25E[2];	// 25E
         float	unk260;		// 260
         float unk264; // 264 // related to rendering a health bar (INI:Gameplay:bHealthBarShowing); see HighProcess::Unk_18
         NiBillboardNode* unk268; // 268 // rendered health bar // hardcoded texture: "Data\\Textures\\Menus\\Misc\\HealthBar3DBW.dds"
         float	unk26C; // 26C
         float unk270; // 270 // INI:GamePlay:fHealthBarEmittanceTime
         UInt32	unk274;		// 274
         UInt8	unk278;		// 278
         UInt8	pad279[3];	// 279
         NiVector3 unk27C;		// 27C
         UInt32	unk288;		// 288
         UInt32	unk28C;		// 28C
         UInt8	unk290;		// 290
         UInt8	pad291[3];	// 291
         float	unk294;		// 294
         UInt32	unk298;		// 298
         UInt32	unk29C;		// 29C
         UInt32	unk2A0;		// 2A0
         UInt32	unk2A4;		// 2A4
         UInt8	unk2A8;		// 2A8
         UInt8	unk2A9;		// 2A9
         UInt8	pad2AA[2];	// 2AA
         float	unk2AC;		// 2AC
         float	unk2B0;		// 2B0
         UInt32	unk2B4;		// 2B4
         UInt8	unk2B8;		// 2B8
         UInt8	unk2B9;		// 2B9
         UInt8	pad2BA[2];	// 2BA
         UInt32	unk2BC = 1;		// 2BC
         UInt32	unk2C0;		// 2C0
         UInt32	unk2C4;		// 2C4
         TESObjectREFR	* actionTarget[kActionType_Max];	// 2C8
         UInt8			actionActive[kActionType_Max];		// 2DC - true if the specified action is running
         UInt8	pad2E1[3];	// 2E1
         TESObjectREFR	* unk2E4;	// 2E4
         UInt8	unk2E8;		// 2E8
         UInt8	pad2E9[3];	// 2E9

         bool IsAttacking()
            {	return currentAction >= kAction_Attack && currentAction <= kAction_AttackBowArrowAttached;	}
         bool IsBlocking()
            {	return currentAction == kAction_Block;	}
         bool IsRecoiling()
            {	return currentAction == kAction_Recoil;	}
         bool IsStaggered()
            {	return currentAction == kAction_Stagger;	}
         bool IsDodging()
            {	return currentAction == kAction_Dodge;	}
         bool IsMovementFlagSet(UInt32 flag)
            {	return (movementFlags & flag) == flag;	}

         MEMBER_FN_PREFIX(HighProcess);
         DEFINE_MEMBER_FN(Constructor, HighProcess&, 0x00628EE0);

         DEFINE_MEMBER_FN(Subroutine00633250, void, 0x00633250, Actor* subject); // handles unk2BC
         DEFINE_MEMBER_FN(Subroutine00628630, void, 0x00628630, Actor* subject, bool); // sets unk2BC if player is not invisible
   };
   static_assert(sizeof(HighProcess) == 0x2EC, "RE::HighProcess is the wrong size!");

   class CombatController : public TESPackage { // sizeof == 0x1C0
      public:
         CombatController();
         ~CombatController();

         struct TargetInfo {
            Actor	* target;
            UInt32	unk04;		// looks like priority. seen values from 0-200
         };

         struct TargetList {
            TargetInfo	* info;
            TargetList	* next;
         };

         struct AvailableSpellExtraInfo {	// for item-based spells, e.g. potions and scrolls
            void		* unk00;
            UInt32		quantity;
            TESForm		* source;	// e.g. for EnchantmentItem on a scroll, a TESObjectBOOK*
            void		* unk0C;	// looks like pointer into owner's ExtraContainerChanges list for this item
         };

         struct AvailableSpellInfo {
            MagicItem				* item;
            AvailableSpellExtraInfo	* extraInfo;
         };

         struct AvailableSpellList {		// all the spells/magic items the actor has available. includes spells resolved from leveled lists
            AvailableSpellInfo	* info;
            AvailableSpellList	* next;
         };

         struct SelectedSpellInfo {
            MagicItem			* item;
            AvailableSpellExtraInfo	* availableInfo;	// for scrolls, potions, points to the entry to this (if any) in available spells list
                                                      // is NULL for spells
         };

         struct AlliesList {
            Actor		* ally;
            AlliesList	* next;
         };

         Actor				* actor;					// 03C
         TargetList			* targets;					// 040
         float				unk044;						// 044
         UInt32				unk048;						// 048
         UInt32				unk04C;						// 04C
         UInt32				unk050;						// 050
         float				unk054;						// 054
         float				unk058;						// 058
         AvailableSpellList	* rangedSpells;				// 05C
         AvailableSpellList	* meleeSpells;				// 060
         AvailableSpellList	* restoreSpells;			// 064
         AvailableSpellList	* buffSpells;				// 068 anything that doesn't fit in the other 3 categories
         UInt32				unk06C; // 06C
         UInt32            unk070; // 0070 // == 0xB if the actor is calmed
         UInt32            unk074;
         UInt32            unk078;
         SelectedSpellInfo	* selectedMeleeSpell;		// 07C
         SelectedSpellInfo	* selectedRangedSpell;		// 080
         SelectedSpellInfo	* selectedRestoreSpell;		// 084
         UInt32				unk088;						// 088
         UInt32				unk08C;						// 08C
         SelectedSpellInfo	* selectedBuffSpell;		// 090
         SelectedSpellInfo	* selectedBoundArmorSpell;	// 094
         SelectedSpellInfo	* selectedBoundWeaponSpell; // 098
         SelectedSpellInfo	* selectedSummonSpell;		// 09C
         UInt32				unk0A0[(0x15C - 0x0A0) >> 2];	// 0A0
         AlliesList			allies;						// 15C
         UInt32				unk164[(0x1C0 - 0x164) >> 2];	// 164

         MEMBER_FN_PREFIX(CombatController);
         DEFINE_MEMBER_FN(TargetsIncludeActor, bool, 0x00613670, Actor* other);

         bool IsCalmed() const { return this->unk070 == 0xB; }
   };

};