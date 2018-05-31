#pragma once
#include "ReverseEngineered/UI/Tile.h"

namespace XXNMenuHelpers {
   struct Slider {
      Slider() {};
      Slider(float a, float b) : valueMin(a), valueMax(b) {};

      RE::Tile* tile = nullptr;
      float     valueMin = 0.0F;
      float     valueMax = 100.0F;

      float Get() const;
      void Set(float);
      void SetBounds(float fMin, float fMax);
      float ToMyRange  (float) const;
      float ToTileRange(float) const;
   };
};