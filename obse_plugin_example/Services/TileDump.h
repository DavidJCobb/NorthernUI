#pragma once
#include "ReverseEngineered/UI/Tile.h"

void TileDump(RE::Tile*);

struct TileCyclicalReferenceChecker {
   static constexpr UInt32 ce_safeMaxLines = 500;

   std::vector<RE::Tile::Value*> found;
   std::string indent;
   UInt32 lineCount;
   //
   bool IsLoggingSafe() const;
   void Log(const char* format, ...);
   void Check(RE::Tile::Value*, bool isRecursing = false);

   RE::Tile::Value* FindCyclical(RE::Tile::Value*, bool isRecursing = false);

   static TileCyclicalReferenceChecker& GetInstance() {
      static TileCyclicalReferenceChecker instance;
      return instance;
   }
};