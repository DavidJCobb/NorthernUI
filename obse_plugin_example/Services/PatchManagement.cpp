#include "PatchManagement.h"
#include "obse_common/SafeWrite.h"

namespace _Hooks {
   namespace FileFinder {
      void Inner() {
         PatchManager::GetInstance().FireEvent(PatchManager::Req::G_FileFinder);
      };
      __declspec(naked) void Outer() {
         _asm {
            push eax; // protect
            push ecx; // protect
            call Inner;
            pop  ecx; // restore
            pop  eax; // restore
            push 0x00A31014; // reproduce patched-over instruction
            mov  edx, 0x0040E687;
            jmp  edx;
         };
      };
      void Apply() {
         WriteRelJump(0x0040E682, (UInt32)&Outer);
      };
   };
   namespace MainMenu {
      void Inner() {
         static bool already = false;
         if (already)
            return;
         already = true;
         auto& man = PatchManager::GetInstance();
         man.FireEvent(PatchManager::Req::G_MainMenu);
         man.LogPending("WARNING: The following patches failed to apply before or at the time the MainMenu opened!");
      };
      __declspec(naked) void Outer() {
         _asm {
            mov  eax, 0x00584640;
            call eax;       // patched-over call to Menu::Menu()
            call Inner;
            mov  eax, 0x005B5F4F;
            jmp  eax;
         };
      };
      void Apply() {
         WriteRelJump(0x005B5F4A, (UInt32)&Outer);
      };
   };
};
PatchManager::PatchManager() {
   //
   // Set up some hooks to watch for game-related requirements
   //
   _Hooks::FileFinder::Apply();
   _Hooks::MainMenu::Apply();
};

void PatchManager::Expect(size_t count) {
   this->pendingPatches.reserve(count);
};
void PatchManager::FireEvent(size_t event) {
   std::vector<PatchFunc> toFire;
   {  // lock scope
      std::lock_guard<decltype(this->lock)> guard(this->lock);
      this->alreadyFired.set(event);
      for (auto it = this->pendingPatches.begin(); it != this->pendingPatches.end(); ++it) {
         auto& p = *it;
         if (!p.func)
            continue;
         p.requirements.set(event, false);
         if (p.requirements.count() == 0) {
            toFire.push_back(p.func);
            p.func = nullptr; // see below
         }
      }
      //
      // Remove after the loop, to cut down on the number of copy/move/etc. operations:
      //
      this->pendingPatches.erase(
         std::remove_if(
            this->pendingPatches.begin(),
            this->pendingPatches.end(),
            [](auto e) {
               return !e.func;
            }
         ),
         this->pendingPatches.end()
      );
   }
   //
   // Execute callbacks after we manage our array, so that if a callback fires an event, 
   // we don't end up with conflicting list edits (for threaded locks) or hangs (for 
   // non-threaded locks).
   //
   for (auto it = toFire.begin(); it != toFire.end(); ++it)
      (*it)();
};
void PatchManager::LogPending(const char* title) {
   if (!this->pendingPatches.size())
      return;
   if (!title)
      title = "Logging pending patches...";
   _MESSAGE("[PatchManager] %s", title);
   {  // lock scope
      std::lock_guard<decltype(this->lock)> guard(this->lock);
      for (auto it = this->pendingPatches.begin(); it != this->pendingPatches.end(); ++it) {
         auto& p = *it;
         if (!p.func)
            continue;
         _MESSAGE(" - %s", p.name);
      }
      _MESSAGE(" - List ends.");
   }
};
void PatchManager::RegisterPatch(const char* const name, PatchFunc f, std::initializer_list<size_t> requirements) {
   if (!requirements.size()) { // if there are no requirements, then just execute the patch immediately
      f();
      return;
   }
   {  // lock scope
      std::lock_guard<decltype(this->lock)> guard(this->lock);
      if (PatchManager::sc_runSafely) {
         std::vector<size_t> finalBits; // impossible for an enum to implicitly convert to its superclass-type
         {  // Handle the case of all of this patch's requirements having been met already. Unlikely, but possible.
            auto it = requirements.begin();
            for (auto it = requirements.begin(); it != requirements.end(); ++it) {
               if (!this->alreadyFired[*it])
                  finalBits.push_back(*it);
            }
         }
         this->pendingPatches.emplace_back(name, f, finalBits);
      } else {
         this->pendingPatches.emplace_back(name, f, requirements);
      }
   }
};