#pragma once
#include "ReverseEngineered/Forms/Actor.h"

namespace RE {
   extern bool*  const bPlayerInVanityMode;
   extern float* const fVanityCurDistance;
   extern float* const fVanityControlX; // player X input / 57 * timestep * GMST:fVanityModeXMult
   extern float* const fVanityControlY; // player Y input / 57 * timestep * GMST:fVanityModeYMult
   extern float* const fAutoVanityModeTimer;
   extern float* const fPlayerCameraPitch;
   extern float* const fPlayerCameraYaw;
   extern bool*  const bChaseCameraResetQueued;

   // Values computed during movement processing; used to control the speed of the walking and 
   // turning animations (with the caveat that the walking animation speed also influences the 
   // speed at which the actor moves).
   extern constexpr float* fPlayerMoveAnimMult = (float*)0x00B14E58; // also affects movement speed
   extern constexpr float* fPlayerTurnAnimMult = (float*)0x00B14E5C; // for Q/W keys

   class PlayerCharacter : public Character { // sizeof == 0x800
      public:
         enum {
            kVTBL = 0x00A73A0C,
         };
         enum {
            kMiscStat_DaysInPrison = 0,
            kMiscStat_DaysPassed,
            kMiscStat_SkillIncreases,
            kMiscStat_TrainingSessions,
            kMiscStat_LargestBounty,
            kMiscStat_CreaturesKilled,
            kMiscStat_PeopleKilled,
            kMiscStat_PlacesDiscovered,
            kMiscStat_LocksPicked,
            kMiscStat_LockpicksBroken,
            kMiscStat_SoulsTrapped,	// 10
            kMiscStat_IngredientsEaten,
            kMiscStat_PotionsMade,
            kMiscStat_OblivionGatesShut,
            kMiscStat_HorsesOwned,
            kMiscStat_HousesOwned,
            kMiscStat_StoresInvestedIn,
            kMiscStat_BooksRead,
            kMiscStat_SkillBooksRead,
            kMiscStat_ArtifactsFound,
            kMiscStat_HoursSlept,	// 20
            kMiscStat_HoursWaited,
            kMiscStat_DaysAsAVampire,
            kMiscStat_LastDayAsAVampire,
            kMiscStat_PeopleFedOn,
            kMiscStat_JokesTold,
            kMiscStat_DiseasesContracted,
            kMiscStat_NirnrootsFound,
            kMiscStat_ItemsStolen,
            kMiscStat_ItemsPickpocketed,
            kMiscStat_Trespasses,	// 30
            kMiscStat_Assaults,
            kMiscStat_Murders,
            kMiscStat_HorsesStolen,

            kMiscStat_Max			// 34
         };
         struct TopicList {
            TESTopic	* topic;
            TopicList	* next;
         };

         PlayerCharacter();
         ~PlayerCharacter();

         // [ vtbl ]
         // +000 = PlayerCharacter
         // +018 = TESChildCell
         // +05C = MagicCaster
         // +068 = MagicTarget
         // +784 = NiTMapBase

         // [ objects ]
         // +01C TESNPC *
         // +03C BSFadeNode *
         // +040 TESObjectCELL *
         // +048 ExtraContainerChanges *
         // +058 HighProcess *
         // +1F4 hkAllCdPointCollector *
         // +570 TESObjectREFR *
         // +5E4 TESTopic *
         // +5F4 TESQuest *
         // +614    float amountFenced
         // +624 SpellItem *
         // +644 BirthSign *
         // +650 TESClass *
         // +6E8 TESRegion *
         // +700 TESObjectREFR *
         // +728 TESWorldSpace *
         // +740 TESWorldSpace *

         // [ data ]
         // +11C haggle amount?
         // +588 UInt8, bit 0x01 is true if we're in third person?
         // +590 UInt8, is time passing?
         // +5A9 UInt8, fast travel disabled
         // +658	UInt32, misc stat array
         // +70C	'initial state' buffer
         //TODO Expose isInSeWorld flag may be useful check OBlivionReloaded definitions.
         void* 		unk104; // 104 // A getter retrives this instead of 5C8 if you're in third person.
         UInt32		unk108[(0x114 - 0x108) >> 2];				// 108
         UInt8  unk114; // 114
         UInt8  unk115; // 115
         bool   isInSEWorld; // 116 // GetPlayerInSEWorld
         UInt8  unk117; // 117
         DialoguePackage* dialoguePackage;						// 118
         UInt32		unk11C[(0x12C - 0x11C) >> 2];				// 11C
         bool      isMovingIntoNewSpace; // 12C // IsPlayerMovingIntoNewSpace
         UInt8      unk12D; // 12D
         UInt8      unk12E; // 12E
         UInt8      unk12F; // 12F
         float		skillExp[21];								// 130	current experience for each skill
         UInt32		majorSkillAdvances;							// 184
         UInt32		skillAdv[21];								// 188 number of advances for each skill
         UInt8		bCanLevelUp;								// 1DC
         UInt8		unk1DD[3];									// 1DD
         Creature* lastRiddenHorse;							// 1E0
         UInt32		unk1E4[(0x200 - 0x1E4) >> 2];				// 1E4
         bool     showQuestItems; // 200 // SetShowQuestItems, GetShowQuestItems
         UInt8    unk201; // 201
         UInt8    unk202; // 202
         UInt8    unk203; // 203
         float		maxAVModifiers[kActorVal_OblivionMax];		// 204
         float		scriptAVModifiers[kActorVal_OblivionMax];	// 324
         UInt32		unk444[(0x570 - 0x444) >> 2];				// 444
         TESObjectREFR* lastActivatedLoadDoor;	// 570 - most recently activated load door
         void*  unk574; // 574 // has a vtbl
         UInt32 unk578[(0x588 - 0x578) >> 2];	// 578
         bool   isThirdPerson;					// 588
         UInt8  unk589; // 589
         UInt8  unk58A; // 58A // set to 1 when GMST:fVanityModeForceDefault is used
         bool   isAutoRunning; // 58B
         bool   isAutoMoving;	// 58C
         UInt32 sleepHours; // 590 // GetPCSleepHours, IsTimePassing (truthy)
         UInt32 unk594;	// 594
         float  unk598; // 598 // related to archery zoom when holding LT/block
         float  unk59C; // 59C // treated similarly to 5A0 and 5A4
         float  unk5A0; // 5A0 // 
         float  unk5A4; // 5A4 // 
         UInt8  unk5A8; // 5A8
         UInt8  unk5A9;
         UInt8  unk5AA;
         UInt8  unk5AB;
         UInt32 unk5AC; // 5AC // LinkedPointerList*?
         void*  unk5B0; // 5B0 // struct, sizeof == 0x54 // freed when running SetInCharGen 0
         UInt8** attributeBonuses;			// 5B4
         UInt16 unk5B8;							// 5B8
         UInt8  unk5BA;
         UInt8  unk5BB;
         UInt32 trainingSessionsUsed;			// 5BC reset on level-up
         bool   unk5C0; // 5C0 // used in sleep/wait checks
         UInt8  unk5C1;
         UInt8  unk5C2;
         UInt8  unk5C3;
         TESObjectREFR* unk5C4; // 5C4 // reference that the player is queued to activate, if any
         void*  unk5C8; // 5C8 // A getter retrives this instead of 104 if you're in first-person.
         ActorAnimData* firstPersonAnimData;		// 5CC
         NiNode* firstPersonNiNode;			// 5D0
         float  unk5D4; // 5D4
         UInt32 unk5D8;							// 5D8
         ActorAnimData* unk5DC; // 5DC
         void* unk5E0; // 5E0 // has a vtbl; its unk40 is a NetImmerse pointer
         TESTopic* unk5E4;						// 5E4
         UInt32 unk5E8;							// 5E8
         tList<QuestStageItem> knownQuestStageItems;	// 5EC
         TESQuest* activeQuest;					// 5F4
         tList<TESObjectREFR*> activeQuestTargets;	// 5F8 targets whose conditions evaluate to true, updated each frame by HUDMainMenu::Update()
         UInt32 unk600;	// 600
         UInt32 unk604;
         bool   unk608; // 608 // related to sleep/wait checks?
         UInt8  unk609;
         UInt8  unk60A;
         UInt8  unk60B;
         UInt32 unk60C;
         UInt8  unk610;							// 610
         UInt8  isAMurderer; // 611
         UInt8  pad612[2];						// 612
         float  amountSoldStolen; // 614
         UInt32 unk618;
         float  unk61C; // 61C // rotation angle; probably yaw; limited to the range [-PI/2, PI/2]
         UInt32 unk620;
         MagicItem* activeMagicItem;				// 624
         TESObjectBOOK* book;						// 628 /MAybe last activated book or last activated skill book 
         NiVector3 unk62C; // 62C
         UInt32		unk638[(0x644 - 0x638) >> 2];	// 638
         BirthSign* birthSign;					// 644
         UInt32		unk648[(0x650 - 0x648) >> 2];	// 648
         TESClass* wtfClass;						// 650 - this is not the player class! use OBLIVION_CAST(this, TESForm, TESNPC)->npcClass
         UInt32		unk654;							// 654
         UInt32		miscStats[kMiscStat_Max];		// 658
         AlchemyItem* alchemyItem;					// 6E0
         UInt8		bVampireHasFed;					// 6E4 returned by vtbl+260, set by vtbl+264
         UInt8		isInCharGen;					// 6E5
         UInt8		pad6E6[2];						// 6E6
         TESRegion* region;						// 6E8
         UInt32 unk6EC[(0x700 - 0x6EC) >> 2];	// 6EC
         float  unk700; // 700
         UInt32 unk704;
         UInt32 unk708;
         UInt32 unk70C;
         UInt32 unk710; // 710 // time in ms at which something occurred (i.e. GetTickCount())
         UInt32 unk714;
         UInt32 unk718;
         UInt8  unk71C;
         UInt8  unk71D; // 71D // bool? related to player or camera yaw
         UInt8  unk71E; // 71E // bool? related to player or camera yaw
         bool   unk71F; // 71F // indicates that some task has been queued
         NiVector3 lastSafePosition; // 720 // last safe position of the player's mount, or the player if no mount; used to enforce border regions
         void*     lastSafeWorld;    // 72C // used in conjunction with lastSafePosition
         UInt32 unk730;
         float  gameDifficultyLevel;			// 734 ranges from -1 to 1
         UInt8  unk738; // 738
         UInt8  unk739;
         UInt8  unk73A;
         UInt8  unk73B;
         UInt32 unk73C[(0x748 - 0x73C) >> 2];	// 73C
         bool   isFlycamEnabled; // 748 // see 006717BB and 006719D9; is the condition for jump at 0067228F
         UInt8  unk749;
         UInt8  unk74A;
         UInt8  unk74B;
         UInt32 unk74C[(0x780 - 0x74C) >> 2];	// 74C
         UInt32 unk780; // 780
         void*  unk784; // 784 // linked list?
         UInt32 unk788[(0x7A0 - 0x788) >> 2];	// 788
         UInt32 unk7A0; // 7A0
         float  requiredSkillExp[21];			// 7A4 total amt of exp needed to increase each skill
         UInt32 unk7F8;							// 7F8
         UInt32 unk7FC;							// 7FC

         bool	SetActiveSpell(MagicItem * item);
         UInt8	GetAttributeBonus(UInt32 whichAttribute) {
            return whichAttribute < kActorVal_Luck ? (*attributeBonuses)[whichAttribute] : -1;
         }
         void	SetAttributeBonus(UInt32 whichAttr, UInt8 newVal) {
            if (whichAttr < kActorVal_Luck)	(*attributeBonuses)[whichAttr] = newVal;
         }
         UInt32 GetSkillAdvanceCount(UInt32 valSkill) {
            return IsSkill(valSkill) ? skillAdv[valSkill - kActorVal_Armorer] : 0;
         }
         void SetSkillAdvanceCount(UInt32 valSkill, UInt32 val) {
            if (IsSkill(valSkill)) {
               skillAdv[valSkill - kActorVal_Armorer] = val;
            }
         }
         UInt32 ModSkillAdvanceCount(UInt32 valSkill, SInt32 mod);
         UInt32 ModMajorSkillAdvanceCount(SInt32 mod) {
            SInt32 adjustedVal = majorSkillAdvances + mod;
            majorSkillAdvances = (adjustedVal > 0) ? adjustedVal : 0;
            return majorSkillAdvances;
         }
         MagicItem* GetActiveMagicItem();
         bool IsThirdPerson() { return isThirdPerson ? true : false; }
         void TogglePOV(bool bFirstPerson);
         void SetBirthSign(BirthSign* birthSign);
         void ChangeExperience(UInt32 actorValue, UInt32 scaleIndex, float baseDelta);
         void ChangeExperience(UInt32 actorValue, float amount);
         float ExperienceNeeded(UInt32 skill, UInt32 atLevel);
         TESClass* GetPlayerClass() const;
         bool SetSkeletonPath(const char* newPath);
         static void UpdateHead(void);	// TODO: investigate further

         MEMBER_FN_PREFIX(PlayerCharacter);
         DEFINE_MEMBER_FN(GetAppropriateNiNode,   NiNode*,        0x006600F0); // gets first-person or third-person node based on third-person state
         DEFINE_MEMBER_FN(HandleBorderRegions,    void,           0x006714E0); // called by the player movement code; listed here for documentation purposes only; do not call
         DEFINE_MEMBER_FN(GetEquippedMagicItem,   MagicItem*,     0x0065D4A0); // used by HUDMainMenu
         DEFINE_MEMBER_FN(GetPlayerAnimData,      ActorAnimData*, 0x0065D750, bool which);
         DEFINE_MEMBER_FN(GetSkillLevel,          UInt32,         0x005F1910, UInt32 skillId);
         DEFINE_MEMBER_FN(GetLuckModifiedSkill,   float,          0x005E0430, UInt32 skillId);
         DEFINE_MEMBER_FN(IsInDialogue,           bool,           0x005E6C10);
         DEFINE_MEMBER_FN(OnPushIntoRegionBorder, void,           0x0066FF10, bool); // called for every frame the player tries to push into a border region; shows a "you can't go that way" message and moves the player or their mount to the lastSafePosition.
         DEFINE_MEMBER_FN(SetFirstPersonCameraEnabled, void, 0x0066C580, bool);
         DEFINE_MEMBER_FN(ShowQuestUpdate,        bool,           0x006697A0, QuestStageItem* stage);
         DEFINE_MEMBER_FN(Subroutine005E6C10,     bool ,          0x005E6C10); // possibly IsInDialogue
         DEFINE_MEMBER_FN(Subroutine006640D0,     UInt8,          0x006640D0);
         DEFINE_MEMBER_FN(UpdateLastSafePosition, void,           0x00664320); // updates lastSafePosition and lastSafeWorld; called per-frame when the player moves and is determined to not be moving into a border region
         //
         // The below are listed for documentation's sake, and probably shouldn't be called.
         //
         DEFINE_MEMBER_FN(HandleArcheryZoom, void, 0x00666670, float maybeSeconds);
         DEFINE_MEMBER_FN(UpdateFlycam,      void, 0x006643A0); // flycam controls are handled in here
         DEFINE_MEMBER_FN(UpdateCamera,      void, 0x0066B710, UInt32);
   };
   static_assert(sizeof(PlayerCharacter) == 0x800, "RE::PlayerCharacter is the wrong size.");
   static_assert(offsetof(PlayerCharacter, miscStats) == 0x658, "RE::PlayerCharacter is the wrong size.");

   extern void ModPCMiscStat(UInt32 statIndex, UInt32 changeBy);
};