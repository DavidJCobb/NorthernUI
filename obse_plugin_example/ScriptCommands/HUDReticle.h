#pragma once
#include "shared.h"
#include "obse/CommandTable.h"

namespace ScriptCommands {
   namespace HUDReticle {
      extern CommandInfo kCommandGetFloat;
      extern CommandInfo kCommandGetString;
      extern CommandInfo kCommandSetFloat;
      extern CommandInfo kCommandSetString;
      extern CommandInfo kCommandHasTrait;
      extern CommandInfo kCommandGetTraitNames;
      extern CommandInfo kCommandGetChildNames;
      extern CommandInfo kCommandPrintTileInfo;

      bool RegisterCommands(const OBSEInterface* obse);
   };
}