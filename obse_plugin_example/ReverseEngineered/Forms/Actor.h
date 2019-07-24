#pragma once
#include "ReverseEngineered/Forms/MobileObject.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"

namespace RE {
   enum AnimGroupType : UInt8 { // seems to be referred to in debug log messages as "actions;" possibly the precursor to Skyrim's Action system?
      kAnimGroup_Idle = 0,
      kAnimGroup_DynamicIdle,
      kAnimGroup_SpecialIdle,
      kAnimGroup_Forward,
      kAnimGroup_Backward,
      kAnimGroup_Left,
      kAnimGroup_Right,
      kAnimGroup_FastForward,
      kAnimGroup_FastBackward,
      kAnimGroup_FastLeft,
      kAnimGroup_FastRight,
      kAnimGroup_DodgeForward,
      kAnimGroup_DodgeBack,
      kAnimGroup_DodgeLeft,
      kAnimGroup_DodgeRight,
      kAnimGroup_TurnLeft,
      kAnimGroup_TurnRight,
      kAnimGroup_Equip,
      kAnimGroup_Unequip,
      kAnimGroup_AttackBow,
      kAnimGroup_AttackLeft,
      kAnimGroup_AttackRight,
      kAnimGroup_AttackPower,
      kAnimGroup_AttackForwardPower,
      kAnimGroup_AttackBackPower,
      kAnimGroup_AttackLeftPower,
      kAnimGroup_AttackRightPower,
      kAnimGroup_BlockIdle,
      kAnimGroup_BlockHit,
      kAnimGroup_BlockAttack,
      kAnimGroup_Recoil,
      kAnimGroup_Stagger,
      kAnimGroup_Death,
      kAnimGroup_TorchIdle,
      kAnimGroup_CastSelf,
      kAnimGroup_CastTouch,
      kAnimGroup_CastTarget,
      kAnimGroup_CastSelfAlt,
      kAnimGroup_CastTouchAlt,
      kAnimGroup_CastTargetAlt,
      kAnimGroup_JumpStart,
      kAnimGroup_JumpLoop,
      kAnimGroup_JumpLand,
      //
      kAnimGroup_Max,
   };
   enum AnimBodyPart {
      kAnimBodyPart_LowerBody = 0,
      kAnimBodyPart_LeftArm   = 1,
      kAnimBodyPart_LeftHand  = 2,
      kAnimBodyPart_RightArm  = 3,
      kAnimBodyPart_SpecialIdle = 4,
      kAnimBodyPart_Unk5 = 0x5, // doesn't have a string; getters treat it as 0; only used by Death animgroup?
      kAnimBodyPart_Unk6 = 0x6, // doesn't have a string; getters treat it as 3; not used by any animgroup?
      //
      kAnimBodyPart_Count = 5,
   };
   enum AnimBodyState {
      kAnimBodyState_Default = 0,
      kAnimBodyState_Sneak   = 1,
      kAnimBodyState_Swim    = 2,
      kAnimBodyState_Fly     = 3,
   };
   enum AnimCombatType : UInt8 {
      kAnimCombatType_None    = 0,
      kAnimCombatType_Unarmed = 1,
      kAnimCombatType_OneHand = 2,
      kAnimCombatType_TwoHand = 3,
      kAnimCombatType_Staff   = 4,
      kAnimCombatType_Bow     = 5,
      kAnimCombatType_Idle    = 6,
   };

   extern const char** s_AnimCombatTypeNames;
   extern const char** s_AnimBodyPartNames;
   extern const char** s_AnimBodyStateNames;
   extern const char** s_NiControllerSequencCycleTypeNames; // TODO: MOVE ME
   extern const char** s_NiControllerSequenceStateNames; // TODO: MOVE ME
   extern const AnimCombatType* s_WeaponTypeToAnimCombatType;

   struct AnimCode {
      UInt16 code = 0;

      // see debug code at 0040A26A, which separates a code and prints its data

      AnimCode() {};
      inline AnimCode(const UInt16 v) : code(v) {};

      inline operator UInt16() const {
         return this->code;
      }
      inline AnimBodyState bodyState() const {
         return (AnimBodyState)(this->code >> 0xC);
      }
      inline AnimCombatType combatType() const {
         return (AnimCombatType)((this->code >> 8) & 0xF);
      }
      inline AnimGroupType type() const {
         return (AnimGroupType)(this->code & 0xFF);
      }
      //
      inline AnimCode toWalkCode() const {
         switch (this->type()) {
            case kAnimGroup_FastForward:
               return this->withGroupType(kAnimGroup_Forward);
            case kAnimGroup_FastBackward:
               return this->withGroupType(kAnimGroup_Backward);
            case kAnimGroup_FastLeft:
               return this->withGroupType(kAnimGroup_Left);
            case kAnimGroup_FastRight:
               return this->withGroupType(kAnimGroup_Right);
         }
         return *this;
      }
      inline AnimCode withBodyState(AnimBodyState bs) const {
         return (*this & 0x0FFF) | (bs << 0x0C);
      }
      inline AnimCode withCombatType(AnimCombatType ct) const {
         return (*this & 0xF0FF) | (ct << 0x08);
      }
      inline AnimCode withGroupType(AnimGroupType gt) const {
         return (*this & 0xFF00) | gt;
      }
   };

   //constexpr AnimCode ce_CodeForNoAnimation = AnimCode(0x00FF);

   class TESAnimGroup { // sizeof == 0x2C // subclass of NiRefObject but doesn't store its own refcount
      public:
         TESAnimGroup();
         ~TESAnimGroup();
         virtual void Dispose(bool free);

         struct AnimGroupInfo { // sizeof == 0x24
            const char* name;   // 00
            UInt8  sequenceType; // 04 // AnimSequenceSingle/AnimSequenceMultiple?
            UInt8  pad[3];
            AnimBodyPart unk08; // 08
            UInt32       unk0C; // 0C // frame indices (i.e. for TESAnimGroup)
            UInt32 unk10[5]; // 10
         };

         struct Unk28 { // possibly Group Sound Properties data? // sounds played at 0051AF70, which is near TESAnimGroup funcs
            float unk00 = 0.0F;
            UInt8 unk04 = 0xFA; // 04
            UInt8 pad05[3];
            float unk08 = 1.0F;
            float unk0C = 0.0F;
         };

         UInt32    unk04 = 0; // 04
         AnimCode  code;      // 08
         UInt16    pad0A;
         UInt32    groupFrameCount; // 0C // count of group frames (Start, Detach, Attack, End, etc)
         float**   groupFrameTimes; // 10 // pointer to float array of group frame times (size numFrames)
         NiVector3 unk14;     // 14 // length of this == speed? speed of what?
         UInt8     unk20 = 0; // 20
         UInt8     unk21 = 0; // 21 // number
         UInt16    pad22;
         UInt32    unk24 = 0; // 24 // number of elements in unk28
         Unk28*    unk28 = nullptr; // 28 // array

         MEMBER_FN_PREFIX(TESAnimGroup);
         DEFINE_MEMBER_FN(GetAnimCode,    AnimCode,       0x0051A9F0); // returns this->code
         DEFINE_MEMBER_FN(GetBodyState,   AnimBodyState,  0x0051AC50);
         DEFINE_MEMBER_FN(GetCombatType,  AnimCombatType, 0x0051AC60);
         DEFINE_MEMBER_FN(GetFrameTime,   float, 0x0051AE20, UInt32 frameIndex); // returns this->frameTimes[frameIndex], with sanity checks (0.0F if any checks fail)
         DEFINE_MEMBER_FN(GetUnk14,       void,  0x0051AED0, NiVector3& out);
         DEFINE_MEMBER_FN(GetUnk14Length, float, 0x0051AED0); // gets the length of vector unk14
         DEFINE_MEMBER_FN(GetUnk20,       decltype(TESAnimGroup::unk20), 0x0051AAD0);
         DEFINE_MEMBER_FN(IsIdle,         bool,  0x0051AF00);
         DEFINE_MEMBER_FN(IsPowerAttack,  bool,  0x0051AAA0);

         //
         // Do not call:
         //
         DEFINE_MEMBER_FN(Constructor,    TESAnimGroup*, 0x0051AB30, AnimCode);
         DEFINE_MEMBER_FN(SetAnimCode,    void,          0x0051AA10, AnimCode); // also resets group frame data
         DEFINE_MEMBER_FN(SetUnk28Length, void,          0x0051ACF0, UInt32 length);
   };
   class AnimSequenceBase {
      public:
         virtual ~AnimSequenceBase();
         virtual void   Unk_01(UInt32) = 0;
         virtual bool   Unk_02(UInt32) = 0;
         virtual bool   IsSingle() = 0; // 03
         virtual BSAnimGroupSequence* GetByIndex(SInt8) = 0; // 04 // does... something... for out-of-bounds or -1 indices, but I don't know what
         virtual UInt8  IndexOf(BSAnimGroupSequence*) = 0; // 0xFF == not found
   };
   class AnimSequenceSingle : public AnimSequenceBase { // sizeof == 0x08
      public:
         virtual ~AnimSequenceSingle();
         virtual void   Unk_01(UInt32);
         virtual bool   Unk_02(UInt32);
         virtual bool   IsSingle(); // 03 // always returns true
         virtual BSAnimGroupSequence* GetByIndex(SInt8); // always returns this->unk04
         virtual UInt8  IndexOf(BSAnimGroupSequence*); // 05 // always returns -1

         BSAnimGroupSequence* unk04;
         UInt32 unk08;
   };
   class AnimSequenceMultiple : public AnimSequenceBase { // sizeof == 0x08
      public:
         virtual ~AnimSequenceMultiple();
         virtual void   Unk_01(UInt32);
         virtual bool   Unk_02(UInt32);
         virtual bool   IsSingle(); // 03 // always returns false
         virtual BSAnimGroupSequence* GetByIndex(SInt8);
         virtual UInt8  IndexOf(BSAnimGroupSequence*); // 05

         NiTListBase<const BSAnimGroupSequence>* unk04; // 04 // should be NiTList<type*> but OBSE doesn't have that defined; doesn't matter, though
         UInt32 unk08; // 08
   };
   class ActorAnimData { // sizeof == 0xDC
      public:
         ActorAnimData();
         ~ActorAnimData();

         UInt32    unk00;    // 00 // actual int
         NiNode*   niNode04; // 04 // for the player, this is expected to be their root node; OBSE has observed BSFadeNode for 3rd-person / NiNode for 1st-person
         NiNode*   niNode08; // 08
         NiVector3 queuedMovement; // 0C
         NiVector3 unk18; // 18 // a position of some kind
         NiNode*   niNodes24[5]; // 24
         AnimCode  currentCodes[kAnimBodyPart_Count]; // 38
         UInt16    pad46;
         SInt32    unk48[kAnimBodyPart_Count]; // 48 // frame indices (i.e. for TESAnimGroup); -1 == unused
         UInt32    unk5C[kAnimBodyPart_Count]; // 5C
         AnimCode  unk70[kAnimBodyPart_Count]; // 70
         UInt32    unk7C[kAnimBodyPart_Count]; // 7C // same type as 5C
         SInt8 unk90; // 90
         UInt8 unk91[3];
         float unk94; // 94 // some sort of time value // valid arg to NiAVObject::ConditionallyUpdatePropsAndControllers
         NiControllerManager*   manager;		//98
         NiTPointerMap<AnimSequenceBase>* map9C; // 9C // the keys in this map are AnimCodes
         BSAnimGroupSequence* animSequences[kAnimBodyPart_Count]; // A0 // currently-playing animations?
         UInt32				unkB4;			//B4
         UInt32				unkB8;			//B8
         float movement; // BC // apparently a distance multiplier for walking/running anims, based on movement speed, etc.
         float attack;   // C0 // gets set to TESObjectWEAP::speed when swinging a weapon
         UInt8 unkC4; // C4 // possibly "disable animation blending" bool
         UInt8 unkC5[3];
         Actor* unkC8; // C8
         void*  unkCC; // CC
         UInt32 unkD0;
         void* unkD4[2]; // D4

         bool FindAnimInRange(UInt32 lowBound, UInt32 highBound = -1);
         bool PathsInclude(const char* subString);

         // MSVC sucks and can compile the CALL_MEMBER_FN version as if the args were (UInt16*, AnimBodyPart)
         inline AnimCode GetCurrentAnimCodeForBodyPart(AnimBodyPart bp) {
            return ThisStdCall(0x00470720, this, static_cast<UInt32>(bp));
         };

         MEMBER_FN_PREFIX(ActorAnimData);
         DEFINE_MEMBER_FN(ContainsSequence,         bool,                 0x00470D00, AnimSequenceBase*);
         DEFINE_MEMBER_FN(FindPlayingSequence,      BSAnimGroupSequence*, 0x00474BD0); // searches all sequences in (manager); just calls FindPlayingSequenceAfter(nullptr)
         DEFINE_MEMBER_FN(FindPlayingSequenceAfter, BSAnimGroupSequence*, 0x00472690, BSAnimGroupSequence* searchAfter); // searches all sequences in (manager); nullptr arg = search all sequences
         DEFINE_MEMBER_FN(GetAnimSequence,          BSAnimGroupSequence*, 0x004706E0, AnimBodyPart index); // index 5 returns animSequences[0]; index 6 returns animSequences[3]
         DEFINE_MEMBER_FN(GetCurrentAnimCodeForBodyPart, AnimCode, 0x00470720, AnimBodyPart);
         DEFINE_MEMBER_FN(GetUnk48Item, UInt32, 0x00470750, AnimBodyPart index); // index 5 is treated like 0; index 6 is treated like 3
         //
         DEFINE_MEMBER_FN(Subroutine00470D20, AnimCode, 0x00470D20, AnimCode, bool isRecursiveCall); // queue animcode arg to play? but it doesn't modify anything; it's just a getter
         DEFINE_MEMBER_FN(StopAnimation,      void,     0x00470FC0, AnimBodyPart, float); // unconfirmed
         DEFINE_MEMBER_FN(ApplyUnk18ToNode08, void,     0x00471230, float, void*); // sets unk08's local translate to unk18 and does something with unk04 and unk08
         DEFINE_MEMBER_FN(Subroutine00472330, SInt16,   0x00472330, AnimCode); // returns length of a TESAnimGroup's unk14
         DEFINE_MEMBER_FN(Subroutine004723A0, bool,     0x004723A0, NiVector3& out_myLengthIsTheAnimDelta, Actor* actor, bool, bool preventVerticalMovement);
         DEFINE_MEMBER_FN(Subroutine00474530, UInt32,   0x00474530, BSAnimGroupSequence*, AnimCode code, SInt32);
         DEFINE_MEMBER_FN(Subroutine00476D10, void,     0x00476D10, Actor* actor, float frameTime, float);
         DEFINE_MEMBER_FN(Subroutine00476260, UInt32,   0x00476260, AnimGroupType, SInt32); // responsible for playing an anim?
         DEFINE_MEMBER_FN(Subroutine00477B60, void,     0x00477B60, AnimCode, UInt32, SInt32); // critical to movement
         //
         DEFINE_MEMBER_FN(Destructor, void, 0x00475B60);
   };
   typedef ActorAnimData Struct00473EB0;

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
         virtual bool	IsOverencumbered() = 0; // 97
         virtual bool	HasVampireFed(void) = 0;
         virtual void	SetVampireHasFed(bool bFed) = 0;
         virtual void	GetBirthSign(void) = 0; //TODO COntrol the OR definition. Doesn't seem right. Possibly return a pointer to a birthsign or an unique identifier.
         virtual void	Unk_9B(void) = 0;
         virtual void	Unk_9C(void) = 0;
         virtual void	Unk_9D(void) = 0;
         virtual void	Unk_9E(void) = 0;
         virtual UInt32 Unk_9F() = 0;
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
         virtual void	Unk_DA(float) = 0;
         virtual void	Unk_DB(void) = 0;
         virtual void	Unk_DC(void) = 0;
         virtual void	Unk_DD(void) = 0;
         virtual void	Unk_DE(void) = 0;
         virtual void	Unk_DF(void) = 0;
         virtual Creature* GetMountedHorse() = 0;	// E0 // returns this->horseOrRider, only for Character
         virtual void	Unk_E1(void) = 0;
         virtual Character* GetRider() = 0; // E2 // returns this->horseOrRider, only for Creature
         virtual void	Unk_E3(void) = 0;
         virtual void	Unk_E4(void) = 0;
         virtual void	Unk_E5(void) = 0;
         virtual void	Unk_E6(void) = 0;
         virtual void	ModExperience(UInt32 actorValue, UInt32 scaleIndex, float baseDelta) = 0;
         virtual void	Unk_E8(void) = 0;
         virtual void	Unk_E9(UInt32, UInt32) = 0;
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
         DEFINE_MEMBER_FN(GetMovementSpeed,            float,  0x005E65B0); // checks movement flags to get the appropriate speed value
         DEFINE_MEMBER_FN(GetRunSpeed,                 float,  0x005E3750);
         DEFINE_MEMBER_FN(GetSwimSpeed,                float,  0x005E3920);
         DEFINE_MEMBER_FN(GetWalkSpeed,                float,  0x005E3590);
         DEFINE_MEMBER_FN(GetWeaponOut, bool, 0x005E0DA0);
         DEFINE_MEMBER_FN(GetSkillMasteryLevelBySkill, UInt32, 0x005F23B0, UInt32 skillActorValue); // see enum RE::SkillMasteryLevel
         DEFINE_MEMBER_FN(IsHorse,                     bool,   0x005E3290);
   };

   class Character : public Actor { // sizeof == 0x104?
      public:
         Character();
         ~Character();
   };

   DEFINE_SUBROUTINE_EXTERN(bool,           AnimGroupTypeIsIdle,    0x0051AAE0, AnimCode);
   DEFINE_SUBROUTINE_EXTERN(AnimBodyState,  BodyStateFromAnimCode,  0x0051A9D0, AnimCode);
   DEFINE_SUBROUTINE_EXTERN(AnimCombatType, CombatTypeFromAnimCode, 0x0051A9E0, AnimCode);
   DEFINE_SUBROUTINE_EXTERN(AnimCode, MakeAnimCode, 0x0051A9B0, AnimBodyState, AnimCombatType, AnimGroupType);

   //
   // 360 movement research:
   //
   //  - Seems like we can influence the animation that plays by shimming the call to 
   //    MakeAnimCode at 005FDE44 or 005FE1BB. Not sure which yet.
   //
};