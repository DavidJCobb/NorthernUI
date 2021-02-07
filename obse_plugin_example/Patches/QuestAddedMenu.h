#pragma once

namespace CobbPatches {
   namespace QuestAddedMenu {
      namespace SuppressDLCSpamRetroactiveFix {
         extern void Execute();
      }
      //
      // Technically, it's just the GenericMenu, but these patches only 
      // relate to it in the context of showing quest updates.
      //
      extern void Apply();
   };
};