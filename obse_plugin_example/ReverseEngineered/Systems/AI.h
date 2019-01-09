#pragma once
#include "obse/GameProcess.h"

namespace RE {
   class Actor;
   class TESObjectREFR;
   class BaseProcess {
      public:
         enum {
            //	RefIDIdx	combatController;
         };
         enum {
            kMovementFlag_Sneaking = 0x00000400, // overridden by kMovementFlag_Swimming
            kMovementFlag_Swimming = 0x00000800,
         };

         BaseProcess();
         ~BaseProcess();

         virtual void	Destructor(void);
         virtual void	Unk_01(void) = 0;
         virtual UInt32	GetProcessLevel(void) = 0;	// 0 - high, 3 - low
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
         virtual void	Unk_18(UInt32 arg0) = 0;
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
         virtual void	Unk_24(UInt32 arg0) = 0;
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
         virtual UInt32	Unk_3A(UInt32 arg0) = 0;
         virtual UInt32	Unk_3B(UInt32 arg0) = 0;
         virtual void*  Unk_3C(UInt32 arg0) = 0;
         virtual ExtraContainerChanges::EntryData* GetEquippedWeaponData(bool arg0) = 0;
         virtual UInt32	Unk_3E(UInt32 arg0) = 0;
         virtual ExtraContainerChanges::EntryData* GetEquippedAmmoData(bool arg0) = 0;
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
         virtual UInt32	Unk_4B(UInt32 arg0) = 0;
         virtual UInt32	Unk_4C(UInt32 arg0) = 0;
         virtual UInt32	Unk_4D(UInt32 arg0) = 0;
         virtual bool	Unk_4E(void) = 0;
         virtual bool	Unk_4F(void) = 0;
         virtual UInt8	GetUnk084(void) = 0;
         virtual void	SetUnk084(UInt8 arg0) = 0;
         virtual bool	GetUnk01E(void) = 0;
         virtual void	SetUnk01E(UInt32 arg0) = 0;
         virtual void	Unk_54(UInt32 arg0, UInt32 arg1, UInt32 arg2, UInt32 arg3) = 0;
         virtual void	SetUnk024(UInt32 arg) = 0;
         virtual void	Unk_56(void) = 0;
         virtual void	Unk_57(void) = 0;
         virtual void	Unk_58(void) = 0;
         virtual void	GetUnk028(float arg) = 0;
         virtual float	SetUnk028(void) = 0;
         virtual void	Unk_5B(void) = 0;
         virtual void	Unk_5C(void) = 0;
         virtual void	Unk_5D(void) = 0;
         virtual void					SetCurrentPackage(TESPackage* Package) = 0;
         virtual void					SetCurrentPackProcedure(TESPackage::eProcedure PackProcedure) = 0; //OR def Note OR definiton say this return a eProcedure
         virtual TESPackage::eProcedure	GetCurrentPackProcedure(void) = 0;  //OR def
         virtual TESPackage*				GetCurrentPackage(void) = 0;	// returns MiddleHighProcess::pkg0C0 if not NULL, else BaseProcess::package
         virtual void					Unk_62(UInt32 arg0) = 0;			// marks ScriptEventList::kEvent_OnPackageDone
         virtual bhkCharacterProxy **	GetCharProxy(bhkCharacterProxy ** characterProxy) = 0;	// increfs and returns the proxy (or sets to NULL)
         virtual void	Unk_64(void * obj) = 0;
         virtual void	Unk_65(void) = 0;
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
         virtual void	Unk_73(void) = 0;
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
         virtual void	Unk_A1(void) = 0;
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
         virtual void	Unk_AE(void) = 0;
         virtual void	Unk_AF(void) = 0;
         virtual UInt32	GetMovementFlags(void) = 0;
         virtual void	Unk_B1(UInt32 actuallyAnInt, bool) = 0;
         virtual void	Unk_B2(UInt32) = 0; // SetMovementFlags?
         virtual void	Unk_B3(Actor*, NiVector3) = 0; // carries out strafing movement?
         virtual UInt32	Unk_B4(void) = 0; // returns int/enum
         virtual void	Unk_B5(void) = 0;
         virtual void	Unk_B6(void) = 0;

         // action is one of kAction_XXX. Returns action, return value probably unused.
         virtual UInt16	SetCurrentAction(UInt16 action, BSAnimGroupSequence* sequence) = 0;
         virtual void	Unk_B8(void) = 0;
         virtual void	Unk_B9(void) = 0;
         virtual UInt8	GetKnockedState(void) = 0;
         virtual void	Unk_BB(void) = 0;
         virtual void	Unk_BC(void) = 0;

         // arg3 is a multiplier, arg4 appears to be base force to apply
         virtual void	KnockbackActor(Actor* target, float arg1, float arg2, float arg3, float arg4) = 0;
         virtual void	Unk_BE(void) = 0;
         virtual UInt8	GetCombatMode(void) = 0;
         virtual UInt8	SetCombatMode(UInt8 CombatMode) = 0;
         virtual UInt8	GetWeaponOut(void) = 0;
         virtual UInt8	SetWeaponOut(UInt8 WeaponOut) = 0;

         virtual void	Unk_C3(void) = 0;
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
         virtual UInt32	Unk_DB() = 0; // returns int/enum
         virtual UInt8	GetSleepState(void) = 0;
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
         virtual void	Unk_11B(void) = 0;
         virtual void	Unk_11C(void) = 0;
         virtual void	Unk_11D(void) = 0;
         virtual void	Unk_11E(void) = 0;
         virtual UInt32 Unk_11F() = 0; // returns enum? if not implemented here, check on HighProcess
         virtual void	Unk_120(void) = 0;
         virtual void	Unk_121(void) = 0;
         virtual void	Unk_122(void) = 0;
         virtual void	Unk_123(void) = 0;
         virtual void	Unk_124(void) = 0;
         virtual void	Unk_125(void) = 0;
         virtual void	Unk_126(void) = 0;
         virtual void	Unk_127(void) = 0;
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