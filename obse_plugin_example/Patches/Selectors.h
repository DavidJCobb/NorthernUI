#pragma once
#include "ReverseEngineered/UI/Tile.h"

extern RE::Tile* g_northernUIDatastore;
extern RE::Tile* g_northernUIStringstore;
extern RE::Tile* g_northernUILocConfigTile;

namespace CobbPatches {
   namespace Selectors {
      void Apply();
   };
};