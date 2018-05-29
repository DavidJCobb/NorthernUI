#pragma once
#include "ReverseEngineered/Forms/MobileObject.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"

namespace RE {
   class Actor : public MobileObject {
      public:
         Actor();
         ~Actor();

         virtual SInt32	GetInfamy(void) = 0;	// 82
         virtual void	Resurrect(UInt8 Unk1, UInt8 Unk2, UInt8 Unk3) = 0; // 83 //OR Defintion
         virtual void	Unk_84(void) = 0;
         virtual void	Unk_85(void) = 0;
         virtual void	Unk_86(void) = 0;
         virtual void	Unk_87(void) = 0;
            //
            // Applies damage based on game difficulty modifier. Difficulty only applies if the actor is not nullptr. 
            // Used for fall damage (null attacker) and melee attacks, but not spell damage.
         virtual void	ApplyDamage(float damage, float arg1, Actor* attacker) = 0; // 88
         virtual void	Unk_89(void) = 0;
         virtual void	ProcessControls(float) = 0; // 8A //OR Definition handles input for PlayerCharacter
         virtual void	Unk_8B(void) = 0;
         virtual void	SetPackageDismount(void) = 0; // 8C //Or Definition
         virtual void	Unk_8D(void) = 0;
         virtual void	Unk_8E(void) = 0;
         virtual void	Unk_8F(void) = 0;
         virtual void	OnAlarmAttack(Actor* attackVictim, UInt32 arg1) = 0;	// 90 tries to send alarm when 'this' attacks victim
         virtual void	Unk_91(void) = 0;
         virtual SInt32 SendTrespassAlarm(TESForm* trespassedRef, TESForm* owner, SInt32) = 0; // 92
         virtual void	Unk_93(void) = 0;
         virtual void	Unk_94(void) = 0;
         virtual void	Unk_95(void) = 0;
         virtual void	Unk_96(void) = 0;
         virtual void	Unk_97(void) = 0;
         virtual bool	HasVampireFed(void) = 0;
         virtual void	SetVampireHasFed(bool bFed) = 0;
         virtual void	GetBirthSign(void) = 0; //TODO COntrol the OR definition. Doesn't seem right. Possibly return a pointer to a birthsign or an unique identifier.
         virtual void	Unk_9B(void) = 0;
         virtual void	Unk_9C(void) = 0;
         virtual void	Unk_9D(void) = 0;
         virtual void	Unk_9E(void) = 0;
         virtual void	Unk_9F(void) = 0;
         virtual void	Unk_A0(void) = 0;	// A0
         virtual SInt32	GetActorValue(UInt32 avCode) = 0;				//Contol Siged or Unsigned				// current, cumulative value
         virtual float	GetAV_F(UInt32 avCode) = 0;
         virtual void	SetAV_F(UInt32 avCode, float val) = 0;							// base value
         virtual void	SetActorValue(UInt32 avCode, UInt32 val) = 0;
         virtual void	ModMaxAV_F(UInt32 avCode, float amt, Actor* arg2) = 0;
         virtual void	ModMaxAV(UInt32 avCode, SInt32 amt, Actor* arg2) = 0;
         virtual void	ApplyScriptAVMod_F(UInt32 avCode, float amt, UInt32 arg2) = 0;	// script cmds Mod/ForceAV
         virtual void	ApplyScriptAVMod(UInt32 avCode, SInt32 amt, Actor* arg2) = 0;
         virtual void	DamageAV_F(UInt32 avCode, float amt, Actor* arg2) = 0;			// modifier <= 0, console ModAV cmd, damage health, etc
         virtual void	DamageAV(UInt32 value, UInt32 amount, UInt32 unk) = 0;
         virtual void	ModBaseAV_F(UInt32 avCode, float amt) = 0;
         virtual void	ModBaseAV(UInt32 avCode, SInt32 amt) = 0;
         virtual void	Unk_AD(void) = 0;
         virtual void	Unk_AE(void) = 0;
         virtual void	Unk_AF(void) = 0;
         virtual void	Unk_B0(void) = 0;	// B0
         virtual void	Unk_B1(void) = 0;
         virtual void	Unk_B2(void) = 0;
         virtual void	Unk_B3(TESObjectREFR* activatedRefr, UInt32 quantity) = 0; // called after Activate by TESForm::Unk33()
         virtual void	Unk_B4(void) = 0;
         virtual void	Unk_B5(void) = 0;
         virtual void	Unk_B6(void) = 0;
         virtual void	Unk_B7(void) = 0;	// AddSpell?
         virtual void	Unk_B8(void) = 0;	// RemoveSpell?
         virtual void	Unk_B9(void) = 0;
         virtual void	Unk_BA(void) = 0;
         virtual void	Unk_BB(void) = 0;
         virtual void	Unk_BC(void) = 0;
         virtual void	Unk_BD(void) = 0;
         virtual void	Unk_BE(void) = 0;
         virtual void	Unk_BF(void) = 0;
         virtual void	Unk_C0(void) = 0;	// C0
         virtual void	Unk_C1(float, UInt32) = 0;
         virtual void	Unk_C2(void) = 0;
         virtual void	Unk_C3(void) = 0;
         virtual void	ManageAlarm(void) = 0; //OR Defninition
         virtual void	Unk_C5(void) = 0;
         virtual void	Unk_C6(void) = 0;
         virtual void	Unk_C7(void) = 0;
         virtual void	AddPackageWakeUp(void) = 0;
         virtual void	Unk_C9(void) = 0;
         virtual void	Unk_CA(void) = 0;
         virtual void	Unk_CB(void) = 0;
         virtual CombatController*	GetCombatController(void) = 0;
         virtual bool	IsInCombat(bool unk) = 0;
         virtual TESForm *	GetCombatTarget(void) = 0;
         virtual void	Unk_CF(void) = 0;
         virtual void	Unk_D0(void) = 0;	// D0
         virtual void	Unk_D1(void) = 0;
         virtual void	Unk_D2(void) = 0;
         virtual void	Unk_D3(void) = 0;
         virtual bool	IsYielding(void) = 0;
         virtual bool	IsTrespassing() = 0;
         virtual void	Unk_D6(void) = 0;
         virtual void	Unk_D7(void) = 0;
         virtual void	Unk_D8(void) = 0;
         virtual void	Unk_D9(void) = 0;
         virtual void	Unk_DA(void) = 0;
         virtual void	Unk_DB(void) = 0;
         virtual void	Unk_DC(void) = 0;
         virtual void	Unk_DD(void) = 0;
         virtual void	Unk_DE(void) = 0;
         virtual void	Unk_DF(void) = 0;
         virtual Creature* GetMountedHorse() = 0;	// E0 returns this->horseOrRider, only for Character
         virtual void	Unk_E1(void) = 0;
         virtual void*	Unk_E2() = 0;
         virtual void	Unk_E3(void) = 0;
         virtual void	Unk_E4(void) = 0;
         virtual void	Unk_E5(void) = 0;
         virtual void	Unk_E6(void) = 0;
         virtual void	ModExperience(UInt32 actorValue, UInt32 scaleIndex, float baseDelta) = 0;
         virtual void	Unk_E8(void) = 0;
         virtual void	Unk_E9(void) = 0;
         virtual void	Unk_EA(void) = 0;
         virtual void	AttackHandling(UInt32 unused, TESObjectREFR* arrowRef, TESObjectREFR * target) = 0;	// args all null for melee attacks
         virtual void	Unk_EC(void) = 0;
         virtual void	Unk_ED(void) = 0;	// something with blocking
         virtual void	OnHealthDamage(Actor* attacker, float damage) = 0;

         // unk1 looks like quantity, usu. 1; ignored for ammo (equips entire stack)
         // itemExtraList is NULL as the container changes entry is not resolved before the call
         void				EquipItem(TESForm * objType, UInt32 unk1, BaseExtraList* itemExtraList, UInt32 unk3, bool lockEquip);
         void				UnequipItem(TESForm* objType, UInt32 unk1, BaseExtraList* itemExtraList, UInt32 unk3, bool lockUnequip, UInt32 unk5);

         UInt32				GetBaseActorValue(UInt32 value);
         EquippedItemsList	GetEquippedItems();
         ExtraContainerDataList	GetEquippedEntryDataList();
         bool				CanCastGreaterPower(SpellItem* power);
         void				SetCanUseGreaterPower(SpellItem* power, bool bAllowUse, float timer = -1);
         void				UnequipAllItems();

         // 8
         struct PowerListData {
            SpellItem	* power;
            float		timer;		// init'ed to (3600 / TimeScale) * 24 <- TimeScale is a TESGlobal
         };

         struct PowerListEntry {
            PowerListData	* data;
            PowerListEntry  * next;

            PowerListData * Info() const { return data; }
            PowerListEntry * Next() const { return next; }
            void SetNext(PowerListEntry* nuNext) { next = nuNext; }
            void Delete();
            void DeleteHead(PowerListEntry* replaceWith);
         };

         // 8
         struct Unk0A4
         {
            // 8+
            struct Data
            {
               UInt32	unk0;
               Actor	* unk4;
            };

            Data	* data;
            Unk0A4	* next;
         };

         // bases
         MagicCaster		magicCaster;					// 05C
         MagicTarget		magicTarget;					// 068

         UInt32 unk070; // 70
         UInt32 unk074; // 74
         bool   aiEnabled; // 78
         UInt8  unk079; // 79
         UInt8  unk07A; // 7A
         UInt8  unk07B; // 7B
         Actor* unk07C;						// 07C
         UInt32 unk080[(0x088 - 0x080) >> 2];	// 080
         ActorValues    avModifiers;      // 088
         PowerListEntry greaterPowerList; // 09C
         Unk0A4 unk0A4;							// 0A4
         float  unk0AC;							// 0AC
         UInt32 DeadState;						// 0B0
         UInt32 unk0B4;							// 0B4
         UInt32 unk0B8;							// 0B8
         float  unk0BC;							// 0BC
         UInt8  unk0C0;							// 0C0
         UInt8  pad0C0[3];						// 0C1
         UInt32 unk0C4;							// 0C4
         bool   forceRun;   // 0C8 // GetForceRun, SetForceRun
         bool   forceSneak; // 0C9 // GetForceSneak, SetForceSneak
         UInt8  unk0CA;							// 0CA
         UInt8  pad0C8;							// 0CB
         TESObjectREFR*	unk0CC;							// 0CC
         UInt32 unk0D0;							// 0D0
         Actor* horseOrRider;					// 0D4 - For Character, currently ridden horse
                                                   //- For horse (Creature), currently riding Character
         UInt32 unk0D8[(0x0E4 - 0x0D8) >> 2];	// 0D8
         Actor* unk0E4;						// 0E4
         UInt32 unk0E8[(0x104 - 0x0E8) >> 2];	// 0E8
                                                      // 104

         TESPackage* GetCurrentPackage();
         bool IsObjectEquipped(TESForm* object);
         float GetAVModifier(eAVModifier mod, UInt32 avCode);
         float GetCalculatedBaseAV(UInt32 avCode);
         bool IsAlerted();
         void SetAlerted(bool bAlerted);
         void EvaluatePackage();
         bool IsTalking();

         MEMBER_FN_PREFIX(Actor);
         DEFINE_MEMBER_FN(GetLuckModifiedSkill,        float,  0x005E0430, UInt32 skillActorValue);
         DEFINE_MEMBER_FN(GetMovementSpeed,            float,  0x005E65B0);
         DEFINE_MEMBER_FN(GetSkillMasteryLevelBySkill, UInt32, 0x005F23B0, UInt32 skillActorValue); // see enum RE::SkillMasteryLevel
         DEFINE_MEMBER_FN(IsHorse,                     bool,   0x005E3290);
   };

   class Character : public Actor { // sizeof == 0x104?
      public:
         Character();
         ~Character();
   };
};