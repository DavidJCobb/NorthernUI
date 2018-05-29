#pragma once
#include "ReverseEngineered/Forms/EffectItem.h"
#include "obse/GameForms.h"

namespace RE {
   class AlchemyItem : public MagicItemObject { // sizeof == 0x80
      public:
         AlchemyItem();
         ~AlchemyItem();

         enum {
            kAlchemy_NoAutocalc = 0x1,
            kAlchemy_IsFood     = 0x2, // in moreFlags
         };

         // MagicItem magicItem;      // 024
         //  |
         //  +-- EffectItemList list; // 024 + 00C
         //
         TESModel model; // 40
         TESIcon  icon;  // 58
         TESScriptableForm scriptable; // 64
         TESWeightForm     weight;     // 70
         UInt32   goldValue; // 78
         UInt32   moreFlags; // 7C
         
         static AlchemyItem* Create(); // creates on the game's memory heap

         MEMBER_FN_PREFIX(AlchemyItem);
         DEFINE_MEMBER_FN(Constructor, AlchemyItem*, 0x00412BE0);
         DEFINE_MEMBER_FN(Destructor,  void,         0x00412880);

         bool IsPoison() const;
         bool IsFood() const { return _IsFlagSet(kAlchemy_IsFood); }
         void SetIsFood(bool bFood) { _SetFlag(kAlchemy_IsFood, bFood); }
         bool IsAutoCalc() const { return !_IsFlagSet(kAlchemy_NoAutocalc); }
         void SetAutoCalc(bool bSet) { return _SetFlag(kAlchemy_NoAutocalc, !bSet); }
         float GetGoldValue();
         void SetGoldValue(UInt32 newValue);
      private:
         bool _IsFlagSet(UInt32 flag) const;
         void _SetFlag(UInt32 flag, bool bSet);
   };
};