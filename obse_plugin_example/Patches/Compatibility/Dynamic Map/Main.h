#pragma once

namespace CobbPatches {
   namespace DynamicMapEmulation {
      //
      // Emulate the functions of Dynamic Map, for compatibility. This only 
      // works if the mod is installed (we're just doing what its scripts do 
      // but in C++; we're still using its definitions and assets).
      //
      void Apply();
   }
};