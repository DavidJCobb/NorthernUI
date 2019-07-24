#pragma once
#include "ReverseEngineered/UI/Tile.h"

void TileDump(RE::Tile*);

struct TileCyclicalReferenceChecker {
   std::vector<RE::Tile::Value*> found;
   //
   void Check(RE::Tile::Value*, bool isRecursing = false);

   static TileCyclicalReferenceChecker& GetInstance() {
      static TileCyclicalReferenceChecker instance;
      return instance;
   }
};