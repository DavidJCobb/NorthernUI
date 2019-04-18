#pragma once

namespace CobbPatches {
   namespace DynamicTrainingCost {
      //
      // Allow TrainingMenu XML to "see" the state of Dynamic Training Cost 
      // script variables, so we can adjust layout accordingly.
      //
      void Apply();
   }
};