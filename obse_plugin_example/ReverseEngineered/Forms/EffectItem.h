#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "obse/GameForms.h"

namespace RE {

   //
   // Misc findings:
   //  - EffectSetting: flag "UnknownN" is likely "ignore during alchemy 'is hostile' checks" (may serve other functions as well)
   //  - EffectSetting: flag "UnknownV" is "magnitude scales with player level"; displayed value is (GMST:fMagicLevelMagnitudeMult * EffectItem magnitude)
   //  - EffectSetting: flag "UnknownW" is "magnitude is a distance/radius in feet"
   //

   class MagicItem;
   class EffectItem { // sizeof == 0x24
      public:
         EffectItem();
         ~EffectItem();

         enum {
            kRange_Self = 0,
            kRange_Touch,
            kRange_Target,
         };

         struct ScriptEffectInfo {
            UInt32    scriptRefID;      // 00
            UInt32    school;           // 04
            BSStringT effectName;       // 08
            UInt32    visualEffectCode; // 10
            UInt8     isHostile;        // 14
            UInt8     padIsHostile[3];  // 15

            void SetName(const char* name);
            void SetSchool(UInt32 school);
            void SetVisualEffectCode(UInt32 code);
            void SetIsHostile(bool bIsHostile);
            bool IsHostile() const;
            void SetScriptRefID(UInt32 refID);

            ScriptEffectInfo* Clone() const;
            void CopyFrom(const ScriptEffectInfo* from);
            static ScriptEffectInfo* Create();
         };

         // mising flags
         UInt32	effectCode;			// 00 // e.g. 'REAT'
         SInt32	magnitude;			// 04
         SInt32	area;				// 08
         SInt32	duration;			// 0C
         UInt32	range;				// 10 // enum
         UInt32	actorValueOrOther;	// 14
         ScriptEffectInfo* scriptEffectInfo;	// 18
         EffectSetting* setting;	// 1C
         float	cost;				// 20 on autocalc items this seems to be the cost

         bool   HasActorValue() const;
         UInt32 GetActorValue() const;
         bool   IsValidActorValue(UInt32 actorValue) const;
         void   SetActorValue(UInt32 actorValue);

         bool   IsScriptedEffect() const;
         UInt32 ScriptEffectRefId() const;
         UInt32 ScriptEffectSchool() const;
         UInt32 ScriptEffectVisualEffectCode() const;
         bool   IsScriptEffectHostile() const;

         EffectItem* Clone() const;
         void CopyFrom(const EffectItem* from);
         static EffectItem* ProxyEffectItemFor(UInt32 effectCode);

         bool operator<(EffectItem*rhs) const;
         // return the magicka cost of this effect item
         // adjust for skill level if actorCasting is used
         float MagickaCost(TESForm* actorCasting = NULL) const;

         void SetMagnitude(UInt32 magnitude);
         void ModMagnitude(float modBy);
         void SetArea(UInt32 area);
         void ModArea(float modBy);
         void SetDuration(UInt32 duration);
         void ModDuration(float modBy);
         void SetRange(UInt32 range);
         bool IsHostile() const;

         static EffectItem* Clone (EffectItem*);
         static EffectItem* Create(EffectSetting*);

         MEMBER_FN_PREFIX(EffectItem);
         DEFINE_MEMBER_FN(Constructor,             EffectItem*, 0x00414790, EffectSetting*);
         DEFINE_MEMBER_FN(CopyConstructor,         EffectItem*, 0x004149A0, EffectItem*);
         DEFINE_MEMBER_FN(FormatDescription,       BSStringT*,  0x004145D0, BSStringT* out, MagicItem* myContainingMagicItem, float seemsUnused); // used for spell, potion, etc. effect descriptions seen in various menus // wraps FormatDescriptionInner; gets the params you don't supply // out MUST be empty or uninitialized
         DEFINE_MEMBER_FN(FormatDescriptionInner,  void,        0x00413BE0, BSStringT* out, UInt32 magicItemVirtualMethod06CallResult, float seemsUnused, bool isApparelEnchant, bool isPotion, bool isWeaponEnchant); // out MUST be empty or uninitialized
         DEFINE_MEMBER_FN(GetDuration,             SInt32, 0x004133C0); // returns 0 if setting's NoDuration flag is set
         DEFINE_MEMBER_FN(GetMagicSchool,          RE::MagicSchool, 0x00412F20);
         DEFINE_MEMBER_FN(GetName,                 void,   0x00413A90, char* out); // e.g. "Restore [Attribute Name]" for a 'REAT' effectCode
         DEFINE_MEMBER_FN(GetScriptedOrActualName, void,   0x004139F0, BSStringT* name); // suitable for internal use, e.g. as the name of a generated tile
         DEFINE_MEMBER_FN(IsHostile,               bool,   0x00413470);
         DEFINE_MEMBER_FN(SameAlchemyEffectAs,     bool,   0x00412EF0, EffectItem*); // checks effect code, skill and attribute flags, and target actor value
         DEFINE_MEMBER_FN(SetDurationIfPossible,   bool,   0x004133E0, SInt32 value); // returns true if changes are made; if changed, cost is forced to -1.0 // caller should enforce a minimum of 1
         DEFINE_MEMBER_FN(SetMagnitudeIfPossible,  bool,   0x00413340, SInt32 value); // returns true if changes are made; if changed, cost is forced to -1.0 // caller should enforce a minimum of 1
         DEFINE_MEMBER_FN(SetAreaIfPossible,       bool,   0x00413390, SInt32 value); // returns true if changes are made; if changed, cost is forced to -1.0 // can be 0; should be 0 for potions and poisons
   };

   class EffectItemList {
      public:
         EffectItemList();
         ~EffectItemList();

         struct Entry {
            EffectItem* effectItem;
            Entry*      next;
         };

         virtual float	GetMagickaCost(Actor* actor);
         virtual SInt32	GetMagickaCost_I(Actor* actor);					// returns NULL in base class impl
                                                                     //virtual UInt32 GetMaxEffectCount()   For coef this vtbl has 4 elements
         virtual UInt32	GetMasteryLevel(void);

         // void			** _vtbl;				// 000
         Entry			effectList;				// 004 BSSimpleList<EffectItem>, from which this class is derived
         UInt32			hostileEffectCount;		// 00C missing from original OBSE class definition

         EffectItem* ItemAt(UInt32 whichItem);
         UInt32 AddItem(EffectItem* effectItem);
         UInt32 AddItemCopy(EffectItem* effectItem);
         UInt32 CopyItemFrom(EffectItemList& fromList, UInt32 whichItem);
         bool HasEffects();
         bool RemoveItem(UInt32 whichItem);
         bool HasNonHostileItem() const;
         bool HasHostileItem() const;
         UInt32 CountItems() const;
         UInt32 CountHostileItems() const;
         EffectItem* ItemWithHighestMagickaCost() const;
         UInt32 GetMagickaCost(TESForm* form = NULL) const;
         const char* GetNthEIName(UInt32 whichEffect) const;
         void RemoveAllItems();
         float CalcGoldValue();
         UInt32 GetSchoolSkillAV();
         UInt32 GetSchoolCode();		// returns one of EffectSetting::kEffect_XXX (values 0 thru 5)

         MEMBER_FN_PREFIX(EffectItemList);
         DEFINE_MEMBER_FN(AddItem,                void,        0x00414B90, EffectItem*);
         DEFINE_MEMBER_FN(Clear,                  void,        0x00414C70); // remove all effects
         DEFINE_MEMBER_FN(FormatCostliestEffectSkillRequirement, BSStringT*, 0x00415390, BSStringT* out);
         DEFINE_MEMBER_FN(GetCostliestEffect,     EffectItem*, 0x004152C0, UInt32, bool);
         DEFINE_MEMBER_FN(GetItem,                EffectItem*, 0x00414B10, UInt32); // alias of GetEffectItemByIndex
         DEFINE_MEMBER_FN(GetEffectItemByIndex,   EffectItem*, 0x00414B10, UInt32);
         DEFINE_MEMBER_FN(HasEffectMatchingAlch,  bool,        0x00414F00, UInt32 effectCode, UInt32 actorValueOrOther);
         DEFINE_MEMBER_FN(IsPoison,               bool,        0x00414EB0);
   };

   class MagicItem : public TESFullName {
      public:
         MagicItem();
         ~MagicItem();

         virtual bool   HasDifferentNameFrom(BaseFormComponent* other); // 03
         virtual bool   ShouldAutocalc() = 0;        // 04
         virtual void   SetShouldAutocalc(bool) = 0; // 05
         //
         // Next function returns an int from an enum.
         // 6: EnchantmentItem
         // 7: AlchemyItem
         //
         virtual UInt32 Unk_06() = 0; // 06 // Returns an enum; for EnchantmentItem, it's 6. for SpellItem, returns spellType
         //
         virtual bool   Unk_07();     // 07 // gets a flag, for SpellItem
         virtual bool   Unk_08();     // 08 // gets a flag, for SpellItem
         virtual UInt32 Unk_09() = 0; // 09 // returns a type code, e.g. 'ENIT' for AlchemyItem and EnchantmentItem or 'SPIT' for SpellItem
         virtual void*  Unk_0A() = 0; // 0A // AlchemyItem returns pointer to its goldValue field? SpellItem returns a pointer to its spellType?
         virtual UInt32 Unk_0B() = 0; // 0B // AlchemyItem returns 8; EnchantmentItem and SpellItem return 16
         virtual bool   HasDifferentFields(MagicItem*) = 0; // 0C
         virtual void   CopyFieldsFrom(MagicItem*) = 0;     // 0D // Requires that both MagicItems be the same kind; checked via dynamic cast.
         virtual void   SaveData();                         // 0E // likely "SaveData", based on AlchemyItem
         virtual void   LoadData(void* probablyRecordData, UInt32 recordName); // 0F // e.g. (void*, 'MODL')

         EffectItemList	list;	// 00C

         enum EType {
            kType_None = 0,
            kType_Spell = 1,
            kType_Enchantment = 2,
            kType_Alchemy = 3,
            kType_Ingredient = 4,
         };
         EType Type() const;
   };
};