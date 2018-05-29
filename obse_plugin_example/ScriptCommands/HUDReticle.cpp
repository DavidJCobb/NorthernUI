#include "HUDReticle.h"

#include "obse/GameAPI.h"
#include "obse/GameMenus.h"
#include "obse/GameTiles.h"

#include "obse/ParamInfos.h"
#include "obse/Script.h"
#include "obse/ScriptUtils.h"

#include "ReverseEngineered/UI/Tile.h"

namespace ScriptCommands {
   namespace HUDReticle {
      #if OBLIVION
         // ==================================================================================================
         //    HELPERS
         // ==================================================================================================
         static Tile* GetHUDReticleTile() {
            auto manager = InterfaceManager::GetSingleton();
            if (manager)
               return manager->hudReticule;
_MESSAGE("GetHUDReticleTile failed");
            return nullptr;
         };
         static Menu* GetHUDReticleMenu() { // BROKEN!!!
            auto tile = GetHUDReticleTile();
            if (tile)
               return RE::Tile::Decorate(tile)->GetContainingMenu();
_MESSAGE("GetHUDReticleMenu failed");
            return nullptr;
         };
         //
         static Tile* ExtractMenuComponent(COMMAND_ARGS, const char* componentNameBuffer) {
            Menu* menu = GetHUDReticleMenu();
            if (menu)
               return menu->GetComponentByName(componentNameBuffer);
_MESSAGE("ExtractMenuComponent failed. componentNameBuffer is %s", componentNameBuffer);
            return nullptr;
         }

         // ==================================================================================================
         //    COMMAND CODE
         // ==================================================================================================

         enum {
            kGetFloat,
            kSetFloat,
            kGetString,
            kSetString,
            kExists
         };

         static bool GetSetMenuValue_Execute(COMMAND_ARGS, UInt32 mode) {
            const char* separatorChar = GetSeparatorChars(scriptObj);
            char stringArg[kMaxMessageLength] = { 0 };
            float newFloatVal = 0;
            char* newStringVal = NULL;

            char* componentPath = stringArg;

            bool bExtracted = false;
            switch (mode) {
               case kGetFloat:
               case kGetString:
               case kExists:
                  bExtracted =
                     g_scriptInterface->ExtractFormatStringArgs(
                        0, stringArg, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList,
                        kCommandGetFloat.numParams
                     );
                  break;
               case kSetFloat:
                  bExtracted =
                     g_scriptInterface->ExtractFormatStringArgs(
                        0, stringArg, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList,
                        kCommandSetFloat.numParams, &newFloatVal
                     );
                  break;
               case kSetString:
                  {
                     bExtracted =
                        g_scriptInterface->ExtractFormatStringArgs(
                           0, stringArg, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList,
                           kCommandSetString.numParams
                        );
                     // extract new value from format string
                     char* context = nullptr;
                     componentPath = strtok_s(stringArg, separatorChar, &context);
                     newStringVal  = strtok_s(nullptr,   separatorChar, &context);
                     bExtracted    = (bExtracted && componentPath && newStringVal);
                  }
                  break;
            }

            char* strToAssign = "";

            if (bExtracted) {
               //Menu* menu = GetHUDReticleMenu();
               Tile* tile = GetHUDReticleTile();
               //if (menu && menu->tile) {
               if (tile) {
                  //Tile::Value* val = menu->tile->GetValueByName(componentPath);
                  Tile::Value* val = tile->GetValueByName(componentPath);
if (!val)_MESSAGE("GetSetMenuValue_Execute - GetValueByName failed. componentPath is %s", componentPath);
                  if (val) {
                     #if _DEBUG && 0
                     val->DumpExpressionList();
                     #endif
                     switch (mode) {
                        case kExists:
                           *result = 1;
                           break;
                        case kGetFloat:
                           if (val->IsNum())
                              *result = val->num;
                           break;
                        case kSetFloat:
                           val->bIsNum = 1;
                           val->parentTile->UpdateFloat(val->id, newFloatVal);
                           break;
                        case kGetString:
                           if (val->IsString())
                              strToAssign = val->str.m_data;
                           break;
                        case kSetString:
                           val->bIsNum = 0;
                           val->parentTile->UpdateString(val->id, newStringVal);
                           val->parentTile->SetStringValue(val->id, newStringVal);
                           val->parentTile->UpdateField(val->id, 0, newStringVal);
                           //
                           break;
                     }
                  }
               }
            }

            if (mode == kGetString)		// need to assign even if errors occur during arg extraction/value retrieval
               AssignToStringVar(PASS_COMMAND_ARGS, strToAssign);

            return true;
         }

         static bool Cmd_GetFloat_Execute(COMMAND_ARGS) {
            return GetSetMenuValue_Execute(PASS_COMMAND_ARGS, kGetFloat);
         }
         static bool Cmd_GetString_Execute(COMMAND_ARGS) {
            return GetSetMenuValue_Execute(PASS_COMMAND_ARGS, kGetString);
         }
         static bool Cmd_SetFloat_Execute(COMMAND_ARGS) {
            return GetSetMenuValue_Execute(PASS_COMMAND_ARGS, kSetFloat);
         }
         static bool Cmd_SetString_Execute(COMMAND_ARGS) {
            return GetSetMenuValue_Execute(PASS_COMMAND_ARGS, kSetString);
         }
         static bool Cmd_HasTrait_Execute(COMMAND_ARGS) {
            *result = 0;
            return GetSetMenuValue_Execute(PASS_COMMAND_ARGS, kExists);
         }

         static bool Cmd_GetTraitNames_Execute(COMMAND_ARGS) {
            if (!ExpressionEvaluator::Active()) {
               ShowRuntimeError(scriptObj, "GetTileTraits must be called within an OBSE expression.");
               return true;
            }
            //
            char buf[kMaxMessageLength] = { 0 };
            UInt32 arrID = g_ArrayMap.Create(kDataType_String, false, scriptObj->GetModIndex());
            *result = arrID;
            //
            Tile* tile = ExtractMenuComponent(PASS_COMMAND_ARGS, buf);
            if (tile) {
               for (Tile::ValueList::Node* node = tile->valueList.start; node && node->data; node = node->next) {
                  const char* traitName = Tile::StrIDToStr(node->data->id);
                  if (node->data->bIsNum)
                     g_ArrayMap.SetElementNumber(arrID, traitName, node->data->num);
                  else
                     g_ArrayMap.SetElementString(arrID, traitName, node->data->str.m_data);
               }
            }
            //
            return true;
         }
         static bool Cmd_GetChildNames_Execute(COMMAND_ARGS) {
            if (!ExpressionEvaluator::Active()) {
               ShowRuntimeError(scriptObj, "GetTileChildren must be called within an OBSE expression.");
               return true;
            }
            //
            char buf[kMaxMessageLength] = { 0 };
            UInt32 arrID = g_ArrayMap.Create(kDataType_Numeric, true, scriptObj->GetModIndex());
            *result = arrID;
            //
            Tile* tile = ExtractMenuComponent(PASS_COMMAND_ARGS, buf);
            if (tile) {
               UInt32 idx = 0;
               for (Tile::RefList::Node* node = tile->childList.start; node && node->data; node = node->next) {
                  g_ArrayMap.SetElementString(arrID, idx, node->data->name.m_data);
                  idx++;
               }
            }
            //
            return true;
         }
         static bool _PrintReticleTileInfo(const char* componentPath, Tile* tile) {
            Console_Print("PrintTileInfo %s", componentPath);
            _MESSAGE("PrintTileInfo %s", componentPath);
            if (!tile) {
               Console_Print("   Component not found");
               _MESSAGE("   Component not found");
               return true;
            }
            //
            Console_Print("  Traits:");
            _MESSAGE("  Traits:");
            std::string output;
            for (Tile::ValueList::Node* node = tile->valueList.start; node && node->data; node = node->next) {
               output = "   ";
               output += Tile::StrIDToStr(node->data->id);
               output += ": ";
               if (node->data->bIsNum) {
                  char num[0x30];
                  sprintf_s(num, sizeof(num), "%.4f", node->data->num);
                  output += num;
               } else
                  output += node->data->str.m_data;

               Console_Print(output.c_str());
               _MESSAGE(output.c_str());
            }
            //
            Console_Print("  Children:");
            _MESSAGE("  Children:");
            for (Tile::RefList::Node* node = tile->childList.start; node && node->data; node = node->next) {
               output = "   ";
               const char* nodeName = node->data->name.m_data;
               output += nodeName ? nodeName : "<UNNAMED NODE>";
               //
               Console_Print(output.c_str());
               _MESSAGE(output.c_str());
            }
            //
            return true;
         }
         static bool Cmd_PrintReticleTileInfo_Execute(COMMAND_ARGS) {
            char componentPath[kMaxMessageLength] = { 0 };
            Tile* tile = ExtractMenuComponent(PASS_COMMAND_ARGS, componentPath);
            return _PrintReticleTileInfo(componentPath, tile);
         }
      #endif

      // ==================================================================================================
      //    PARAM AND COMMAND DEFINITIONS
      // ==================================================================================================

      static ParamInfo kParams_GetMenuValue[SIZEOF_FMT_STRING_PARAMS] = {
         FORMAT_STRING_PARAMS
      };
      static ParamInfo kParams_SetMenuFloatValue[SIZEOF_FMT_STRING_PARAMS + 1] = {
         FORMAT_STRING_PARAMS,
         { "new value",	kParamType_Float,	0 },
      };

      static CommandInfo kCommandGetFloat = {
         "NUI_GetHUDReticleMenuFloat",
         "",
         0,
         "Returns the value of some trait on or inside of the HUDReticle menu, as a float.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_GetFloat_Execute)
      };
      static CommandInfo kCommandGetString = {
         "NUI_GetHUDReticleMenuString",
         "",
         0,
         "Returns the value of some trait on or inside of the HUDReticle menu, as a string.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_GetString_Execute)
      };
      static CommandInfo kCommandSetFloat = {
         "NUI_SetHUDReticleMenuFloat",
         "",
         0,
         "Sets the value of some trait on or inside of the HUDReticle menu, as a float.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_SetMenuFloatValue),
         kParams_SetMenuFloatValue,
         //
         HANDLER(Cmd_SetFloat_Execute)
      };
      static CommandInfo kCommandSetString = {
         "NUI_SetHUDReticleMenuString",
         "",
         0,
         "Sets the value of some trait on or inside of the HUDReticle menu, as a string.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_SetString_Execute)
      };
      static CommandInfo kCommandHasTrait = {
         "NUI_GetHUDReticleMenuHasTrait",
         "",
         0,
         "Returns 1 if the specified trait exists on or inside of the HUDReticle menu (as specified).",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_HasTrait_Execute)
      };
      static CommandInfo kCommandGetTraitNames = {
         "NUI_GetHUDReticleTileTraits",
         "",
         0,
         "Returns an array of trait names on the HUDReticle menu or some specified component inside of the HUDReticle menu.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_GetTraitNames_Execute)
      };
      static CommandInfo kCommandGetChildNames = {
         "NUI_GetHUDReticleTileChildren",
         "",
         0,
         "Returns an array of child tile names on the HUDReticle menu or some specified component inside of the HUDReticle menu.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_GetChildNames_Execute)
      };
      static CommandInfo kCommandPrintTileInfo = {
         "NUI_PrintHUDReticleTileInfo",
         "",
         0,
         "Debug command for printing trait and child tile names on the HUDReticle menu or some specified component inside of the HUDReticle menu.",
         //
         0,	// no parent object
         NUM_PARAMS(kParams_GetMenuValue),
         kParams_GetMenuValue,
         //
         HANDLER(Cmd_PrintReticleTileInfo_Execute)
      };

      // ==================================================================================================
      //    REGISTER
      // ==================================================================================================

      bool RegisterCommands(const OBSEInterface* obse) {
         obse->RegisterCommand(&kCommandGetFloat);
         obse->RegisterCommand(&kCommandGetString);
         obse->RegisterCommand(&kCommandSetFloat);
         obse->RegisterCommand(&kCommandSetString);
         obse->RegisterCommand(&kCommandHasTrait);
         obse->RegisterCommand(&kCommandGetTraitNames);
         obse->RegisterCommand(&kCommandGetChildNames);
         obse->RegisterCommand(&kCommandPrintTileInfo);
         return true;
      }
   }
};