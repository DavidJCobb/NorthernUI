#pragma once
#include "ReverseEngineered/UI/Tile.h"

extern RE::Tile* g_northernUIDatastore;
extern RE::Tile* g_northernUIStringstore;
extern RE::Tile* g_northernUILocConfigTile;
extern RE::Tile* g_northernUIPrefstore;

namespace CobbPatches {
   namespace Selectors {
      void Apply();
   };
};