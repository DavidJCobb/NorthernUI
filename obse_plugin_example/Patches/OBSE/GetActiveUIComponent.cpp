#include "GetActiveUIComponent.h"
#include "shared.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/CommandTable.h"
#include "obse/StringVar.h"

namespace {
   constexpr UInt32 ce_opcode_auto = 0;
   //
   inline RE::Tile* _getActiveTile() {
      auto ui = RE::InterfaceManager::GetInstance();
      if (ui->altActiveTile)
         return ui->altActiveTile; // used for keynav
      return ui->activeTile;
   }
   //
   static bool Patch_GetActiveUIComponentName_Execute(COMMAND_ARGS) {
      const char* name = "";
      auto tile = _getActiveTile();
      if (tile)
         name = tile->name.m_data;
      return g_Str->Assign(PASS_COMMAND_ARGS, name);
   }
   static bool Patch_GetActiveUIComponentFullName_Execute(COMMAND_ARGS) {
      const char* name = "";
      std::string nameStr;
      auto tile = _getActiveTile();
      if (tile) {
         nameStr = ((::Tile*)tile)->GetQualifiedName(); // OBSE-specific member function
         name    = nameStr.c_str();
      }
      return g_Str->Assign(PASS_COMMAND_ARGS, name);
   }
   static bool Patch_GetActiveUIComponentID_Execute(COMMAND_ARGS) {
      *result = -1.0;
      auto tile = _getActiveTile();
      if (tile) {
         float id;
         if (tile->GetFloatValue(kTileValue_id, &id))
            *result = id;
      }
      return true;
   }
   //
   CommandInfo kShimmedInfo_GetActiveUIComponentName = {
	   "GetActiveUIComponentName",
	   "",
      ce_opcode_auto,
	   "returns the name of the highlighted UI component such as a button",
	   0,
	   0,
	   nullptr,
	   Patch_GetActiveUIComponentName_Execute,
	   Cmd_Default_Parse,
      nullptr,
	   0
	};
   CommandInfo kShimmedInfo_GetActiveUIComponentFullName = {
      "GetActiveUIComponentName",
      "",
      ce_opcode_auto,
      "returns the qualified name of the highlighted UI component",
      0,
      0,
      nullptr,
      Patch_GetActiveUIComponentFullName_Execute,
      Cmd_Default_Parse,
      nullptr,
      0
   };
   CommandInfo kShimmedInfo_GetActiveUIComponentID = {
      "GetActiveUIComponentID",
      "",
      ce_opcode_auto,
      "returns the ID of the highlighted UI component as defined in the menu XML",
      0,
      0,
      nullptr,
      Patch_GetActiveUIComponentID_Execute,
      Cmd_Default_Parse,
      nullptr,
      0
   };
}

namespace CobbPatches {
   namespace OBSE {
      namespace GetActiveUIComponent {
         //
         // OBSE's GetActiveUIComponentName function doesn't check the 
         // active UI tile correctly: it only tries to grab the mouse-
         // focused tile, when Oblivion also allows keyboard navigation 
         // and tracks keyboard focus separately. The related functions, 
         // GetActiveUIComponentID and GetActiveUIComponentFullName, are 
         // similarly broken.
         //
         // The most backward-compatible fix here is to just clobber 
         // the functions entirely, to make them behave as was intended.
         //
         void _overwrite(const char* name, CommandInfo& info) {
            //
            // do NOT use OBSECommandInterface::Replace; it is an UNOFFICIAL EXTENSION
            //
            auto op = const_cast<CommandInfo*>(g_commandInterface->GetByName(name));
            if (op) {
               _MESSAGE("Working on %s (opcode %04X, pointer %08X)...", op->longName, op->opcode, op);
               info.opcode = op->opcode;
               memcpy(op, &info, sizeof(CommandInfo));
               _MESSAGE("Replaced %s.", name);
            } else
               _MESSAGE("Unable to locate %s.", name);
            /*//
            auto op = const_cast<CommandInfo*>(g_commandInterface->GetByName(name));
            if (op) {
               _MESSAGE("Working on %s (%04X)...", op->longName, op->opcode);
               op->execute = info.execute;
               _MESSAGE("Replaced %s.", name);
            } else
               _MESSAGE("Unable to locate %s.", name);
            //*/
         }
         //
         void Apply() {
            if (!g_commandInterface)
               return;
            _MESSAGE("Attempting to patch some OBSE commands that are known to handle the UI incorrectly...");
            _overwrite("GetActiveUIComponentID",       kShimmedInfo_GetActiveUIComponentID);
            _overwrite("GetActiveUIComponentName",     kShimmedInfo_GetActiveUIComponentName);
            _overwrite("GetActiveUIComponentFullName", kShimmedInfo_GetActiveUIComponentFullName);
            _MESSAGE("Attempts complete.");
         }
      }
   };
};