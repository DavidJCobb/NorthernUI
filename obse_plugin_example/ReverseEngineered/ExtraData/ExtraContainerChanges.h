#pragma once
#include "obse/GameExtraData.h"

namespace RE {
   class Actor;
   class ExtraContainerChanges : public ::ExtraContainerChanges {
      public:
         struct EntryData : public ::ExtraContainerChanges::EntryData {
            MEMBER_FN_PREFIX(EntryData);
            DEFINE_MEMBER_FN(Destructor,     void,        0x00484470);
            DEFINE_MEMBER_FN(GetCharge,      float,       0x004849C0);
            DEFINE_MEMBER_FN(GetHealth,      float,       0x00484850, bool returnAbsolute); // if arg is false, returns percentage (e.g. 66.0F) // -1 if no valid value
            DEFINE_MEMBER_FN(GetIcon,        const char*, 0x004851B0, Actor*); // icon -- but it checks ownership, tho??
            DEFINE_MEMBER_FN(GetName,        const char*, 0x00488DF0);
            DEFINE_MEMBER_FN(GetOwner,       TESForm*,    0x00484B70);
            DEFINE_MEMBER_FN(GetUses,        SInt32,      0x00484970); // -1 if no valid value
            DEFINE_MEMBER_FN(IsEquippedItem, bool,        0x00484E80, UInt32);
            DEFINE_MEMBER_FN(SetCount,       void,        0x0060D020, UInt32 count);

            // Used to filter inventory displays for menus.
            //
            // Checks:
            //  - Item either is not armor, or is playable
            //  - If item is a quest item, quest items must be hidden (SetShowQuestItems) or (container) must not be the player
            //  - If (container) is not the player, the item must not be bound armor?
            //  - (arg4) is true (returns true; skips all further checks)
            //  - (isBartering) is true or the item is not equipped
            //  - A check I don't understand, related to the item's ownership and "responsibility" values
            //  - The following conditions:
            //       TESActorBase of (container) offers services related to the item
            //    || player's Mercantile skill allows sale of any non-stolen item to any vendor
            //    && item is not equipped or is not poisoned (unless we're actually checking ExtraCannotWear for the latter?)
            //
            DEFINE_MEMBER_FN(Subroutine004854F0, bool, 0x004854F0, Actor* container, bool isBartering, SInt32 gameHourOrMaybeBool, bool arg4, bool alwaysZero);

            // Used to render columns in the inventory menu. Can return nullptr.
            //
            // which == 0 : Count. Values between 1000 and 999999 are rendered as %iK. Values above 999999 are rendered as %iM. Zero renders as a space.
            // which == 1 : Value.
            // which == 2 : Weight.
            // which == 3 : Quality.
            // which == 4 : Health percentage or uses.
            //
            // All values except Count render the same way: negative values return a null string, and values 
            // between zero and one are stringified as "%.1f". Values greater than or equal to one are truncated 
            // to an integer and then returned.
            //
            // Meanings of the booleans aren't known. The InventoryMenu always uses { true, false, false, false }.
            //
            DEFINE_MEMBER_FN(Subroutine0048F450, const char*, 0x0048F450, UInt32 which, bool, bool, bool, bool);
         };
   };
};