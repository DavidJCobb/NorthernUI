#pragma once
#include <string>
#include <unordered_map>
#include <vector>

/*//

   = UI PREFS =

   USE CASE:

   I want to be able to add new settings to the menus, and allow the user to change 
   these settings from XXNOptionsMenu, without having to hardcode all of them. I 
   want to be able to define the settings purely in XML, reference them purely in 
   XML, and add XXNOptionsMenu widgets to edit them purely in XML.

   BASIC IDEA:

   We load a series of "pref" files that define multiple named prefs. We retain 
   these pref definitions -- consisting of a name and a default float value -- in 
   a singleton.

   Prefs' current values are synchronized to a special tile, xxnPrefs(), allowing 
   tiles to pull from them. A group of operators is offered to allow UI XML to 
   trigger changes to prefs' values based on things like a tile's "clicked" trait.

   RELEVANT QUESTIONS (AND MISTAKES TO AVOID):

    = Why not use an operator to retrieve a pref's value as well?
      
      Let me clarify that I mean the difference between these two things:

         <xxnOpPrefLoadValue>MyPref</xxnOpPrefLoadValue>

         <copy src="xxnPrefs()" trait="_MyPref" />
      
      There would be no way to live-update traits that pull a pref's value direct-
      ly from the singleton using a special operator. However, operators that pull 
      from another trait (which we synchronize to) using SRC/TRAIT can be updated 
      using the systems that already exist in Oblivion's UI engine; simply updating 
      the trait that they pull from will trigger an update of the operators. This, 
      incidentally, is also the only reason we push pref values to a tile at all.

    = Why do we only *fully* commit changes to a pref when the menu that instigated 
      those changes is closed?

      Sliders, mainly. When the executable wants to programmatically update a 
      slider or scrollbar's value, it does three things:

       - It sets a specific trait to -999999, which causes the scrollbar's value 
         to be clamped to its minimum.

       - It sets that trait to the desired value, which pushes the scrollbar's 
         value to the desired value.

       - It sets that trait back to 0, so that further changes to the scrollbar 
         are no longer influenced by it.

      So every frame you spend dragging a scrollbar thumb, the game triggers 
      three updates to the scrollbar's value -- which is three pref-modify 
      operators assuming similar markup.

      Clicks aren't much better. The game sets the "clicked" trait on the appro-
      priate scrollbar button to 1 and then back to 0 on the same frame, which 
      is two pref-modify operators.

      Seems cleaner to just go with whatever value is in-place when the menu is 
      closed. That said, we *do* have to push all changes to the pending value to 
      the UI layer so that the slider/scrollbar/checkbox/widget itself can show 
      the pending value.
   
    = Why do we only offer an operator to "modify" a pref's value, and not one to 
      directly "set" a pref's value?

      A "set" operator would only be useful if we allowed XML to manage state, as 
      is done for the values of sliders and scrollbars in the vanilla game. XML can 
      manage state by using traits that never overwrite their last-stored values 
      with <copy /> operators; however, these traits always initialize to zero. 
      It is impossible to set the initial values of these traits to anything else 
      due to how the game actually populates/computes/fills/etc. operator operands 
      at run-time.

      Consider a trait like this:

         <trait>
            <operatorThatReturnsAValueOnlyOnceAndThenDoesNothing>
               <someHypotheticalLoadPrefOperator src="otherTile" trait="_prefName" />
            </operatorThatReturnsAValueOnlyOnceAndThenDoesNothing>
            <add  src="otherTile" trait="clicked" />
            <someHypotheticalSetPrefOperator src="otherTile" trait="_prefName" />
         </trait>

      When an operator is parsed and created, its containing trait is told to 
      immediately recompute its value. This means that the <trait /> trait here 
      actually computes its value multiple times before its containing tile has 
      finished loading. The game also tells all traits in a tile to recompute 
      when the tile itself is finished loading -- just for good measure.

      If any SRC operator in this trait experiences an update, then the trait 
      will recompute at that time as well.

      The (otherTile) you see referenced in there may not have loaded yet, in 
      which case any operator referencing it has an operand of zero. This means 
      that our "run once" operator will always return zero. Eventually, the 
      trait in (otherTile) will load in, causing our <trait /> trait to recompute, 
      but by then, our "run once" operator has already fired and disabled itself 
      from running again.

      The fact of the matter is, there's no way to know when all SRC operators 
      inside of a trait have resolved their values, and so there's no way to know 
      when it's safe to run any sort of "run once" operator. Without a "run once" 
      operator, you can't initialize an XML trait that maintains state to any 
      value other than zero. This means that widgets for changing UI prefs, like 
      sliders and checkboxes, wouldn't be able to display the pref's current 
      value when their containing menu is opened.

      If we forego a direct "set" operator and just use a "modify" operator, with 
      one or two other specialty operators as needed, then we can use a setup 
      like this:

         <_setter>
            <copy src="otherTile" trait="clicked" />
            <xxnOpPrefModifyValue src="otherTile" trait="_prefName" />
         </_setter>
         <trait>
            <copy src="xxnPrefs()" trait="Pref Name Here" />
         </trait>
      
      In this case, XML doesn't maintain state at all. We just tell the game 
      when to make changes. If "clicked" is zero, then we change the pref by 
      zero.

    = Why do we use operators for specific pref operations, like add ("modify"), 
      clamp-to-min, clamp-to-max, modulo, and "carousel?"

      Consider the case of a checkbox: in order to map its value to true and 
      false, we'd want a special variant on a modulo operator: we'd want to 
      leave the value unchanged if it equals 2, but modulo it otherwise. So 
      why can't we implement it in XML? Well, consider this:

         <_setter>
            <copy src="xxnPrefs()" trait="_Pref" />
            <sub>
               <not src="xxnPrefs()" trait="_Pref" />
            </sub>
            <mult>-1</mult>
            <mult src="otherTile" trait="clicked" />
            <xxnOpPrefModifyValue>_Pref</xxnOpPrefModifyValue>
         </_setter>
         <trait>
            <copy src="xxnPrefs()" trait="_Pref" />
         </trait>

      In theory, that should achieve what we want, right? That should toggle 
      the value between 2 and 1, by subtracting 1 whenever the current pref 
      value is 2 and adding 1 whenever the current pref value is 1. However, 
      there's a subtle problem here.

      When you click on a tile, the game sets the "clicked" trait to 1, and 
      then sets it back to 0, all on the same frame. When the game finishes 
      changing a trait's value -- including all of the other operators that 
      have to update in response -- we process any pref value changes that 
      occurred and propagate those changes to the UI. So here's the sequence 
      of events that occurs with the above markup:

       - The game sets "clicked" to 1. The _setter trait contains an operator 
         that refers to the "clicked" trait, so the _setter trait is forced 
         to recompute its value, executing all of its operators in sequence. 
         This flips the checkbox pref's value between true and false.
      
       - The operators in _setter make changes to a pref's value.

       - The game finishes processing all traits and operators that had to 
         update in response to "clicked" being set to 1. We now push all of 
         the pref changes that were made to the UI...

       - ...which entails setting the "_Pref" trait on xxnPrefs(). The 
         _setter trait contains an operator that refers to that "_Pref" 
         trait, so the _setter trait is forced to recompute its value, 
         executing all of its operators in sequence.
      
       - The operators in _setter make changes to a pref's value. Our code 
         has now run twice in a row, with the second run totally cancelling 
         the first run out. We flipped the boolean twice, you see.

       - The game finishes processing all traits and operators that had to 
         update in response to the "_Pref" trait changing. Our particular 
         hook here has guards against recursion, or else we would end up 
         trapped in infinite recursion at this point.

       - The game sets "clicked" to 0. This kicks off the same sequence of 
         events as when "clicked" was set to 1, except that since we MULT 
         everything by "clicked," we just end up modifying the pref's value 
         by zero twice in a row (and we optimize that out in the "modify" 
         operator's code).

      So prefs can only be modified with hardcoded operators: if the same 
      trait that tries to modify a pref's value also pulls that pref's 
      value (however directly or indirectly), then the modification goes 
      wrong.

//*/

namespace cobb {
   struct XMLDocument;
};

#define XXN_UI_ALL_PREFS_ARE_FLOATS 1

class UIPrefManager {
   private:
      UIPrefManager(); // patches the engine for a menu-close hook
   public:
      static UIPrefManager& GetInstance() {
         static UIPrefManager instance;
         return instance;
      };

      struct Pref {
         float       defaultFloat = 0.0F;
         float       currentFloat = 0.0F;
         float       pendingFloat = 0.0F;
         UInt32      pendingChangesFromMenuID = 0; // what menu has written pending changes?

         bool  commitPendingChanges(UInt32 closingMenuID); // returns true if there were changes to commit
         float getValue(UInt32 askingMenuID) const;

         Pref() {}
         Pref(float defaultFloat);
      };
      //
   private:
      std::unordered_map<std::string, Pref> prefs;
      std::vector<std::string> prefsPendingPushToUI;
      //
      Pref* getPrefByName(std::string name);
      const Pref* getPrefByName(std::string name) const;
      //
      void setupDocument(cobb::XMLDocument&);
      void processDocument(cobb::XMLDocument&);
      //
   public:
      float getPrefCurrentValue(const char* name, UInt32 askingMenuID = 0) const;
      float getPrefDefaultValue(const char* name) const;

      void resetPrefValue(const char* name, UInt32 menuID);
      void setPrefValue(const char* name, float v, UInt32 menuID);
      void modifyPrefValue(const char* name, float v, UInt32 menuID);
      void clampPrefValue(const char* name, float v, bool toMin, UInt32 menuID);

      void dumpDefinitions() const;
      void loadDefinitions();

      void onMenuClose(UInt32 menuID);

      void loadUserValues();
      void saveUserValues() const;

      void queuePrefPushToUIState(const std::string& name);
      void pushPrefToUIState(const std::string& name) const;
      void pushQueuedPrefsToUIState();
      void pushAllPrefsToUIState() const;

      static void initialize(); // call this on startup when the game's UI engine has registered all XML entities
};