#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace XXNMenuHelpers {
   struct Checkbox {
      Checkbox() {};
      Checkbox(bool a) : requiresRestart(a) {};

      enum {
         kTraitID_CheckboxState = kTileValue_user20,
      };

      RE::Tile* tile  = nullptr;
      bool      state;
      bool      requiresRestart = false;

      inline bool Checkbox::Get() const {
         return this->state;
      };
      bool HandleClick(RE::Tile* tile); // returns true if event handled; false if event not relevant
      void Render() const;
      void Set(bool);
   };
};