#pragma once
#include <bitset>
#include <mutex>

//
// System for queueing up all patches when the DLL loads, and then firing them off 
// when their prerequisites are met. PatchManager maintains hooks for some game-
// related prerequisites (the hooks are patched-in in the constructor), and when 
// these prerequisites are met, any queued patches waiting for them are fired.
//

class PatchManager {
   private:
      //
      // sc_runSafely:
      //
      // If true, PatchManager double-checks patches when registering them in case 
      // any requirements are already met. In our case, it's needed because some 
      // patches are requirements for other patches, and I don't want to have to 
      // care about the order in which they are queued.
      //
      static constexpr bool sc_runSafely = true;
      //
   public:
      class Req { // impossible to implicitly convert enums to underlying types e.g. for std::initializer_list<under>, so we're going with this trash
         Req() = delete;
         //
         public:
            //
            // enum keys:
            //  - G_Something = game-related event, or post-init of a game service
            //  - P_Something = one of my patches
            //  - X_Something = OBSE events and similar
            //
            static constexpr size_t G_FileFinder   = 0; // Has the game's g_FileFinder been created yet?
            static constexpr size_t P_XInput       = 1; // Have we applied the XInput patch (or chosen not to) yet?
            static constexpr size_t X_PostLoad     = 2; // OBSE PostLoad event; use to slip in after most other plug-ins e.g. MenuQue.
            static constexpr size_t G_MainMenu     = 3; // When the main menu opens for the first time
            static constexpr size_t P_MenuQue      = 4; // Have we dealt with MenuQue yet?
            static constexpr size_t P_Selectors    = 5;
            static constexpr size_t COUNT          = 6;
      };
   private:
      PatchManager();
      //
      typedef void(*PatchFunc)();
      struct Patch {
         Patch(const char* name, PatchFunc func, std::initializer_list<size_t> r) : name(name), func(func) {
            for (auto f : r)
               this->requirements.set(f);
         };
         Patch(const char* name, PatchFunc func, std::vector<size_t> r) : name(name), func(func) {
            for (auto f : r)
               this->requirements.set(f);
         };
         //
         const char* name = ""; // can't be const char* const -- that would prevent std::vector from moving elements when there are removals from the middle
         PatchFunc   func = nullptr;
         std::bitset<Req::COUNT> requirements;
      };
      std::vector<Patch> pendingPatches;
      std::bitset<Req::COUNT> alreadyFired;
      std::mutex lock;
      //
   public:
      static PatchManager& GetInstance() {
         static PatchManager instance;
         return instance;
      };
      //
      void Expect(size_t count); // std::vector::reserve
      void FireEvent(size_t event);
      void LogPending(const char* title = nullptr); // logs nothing if no pending patches. good for detecting if a patch has wrongfully failed to apply by a certain point
      void RegisterPatch(const char* const name, PatchFunc f, std::initializer_list<size_t> requirements);
};