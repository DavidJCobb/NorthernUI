#include "OBSESamples.h"

#include "obse/CommandTable.h"
#include "obse/GameAPI.h"
#include "obse/GameObjects.h"

#include "obse/ParamInfos.h"
#include "obse/Script.h"
#include "obse/ScriptUtils.h"

// ==================================================================================================
//    SERIALIZED DATA
// ==================================================================================================

std::string	g_strData;
bool ScriptCommands::OBSESamples::SerializedData01::Save(SerializationHelper* writer) {
   writer->Write(g_strData);
   return true;
};
bool ScriptCommands::OBSESamples::SerializedData01::Load(SerializationHelper* reader, UInt32 version, UInt32 length) {
   g_strData.clear();
   //
   char buf[512];
   reader->ReadBuffer(buf, length);
   buf[length] = 0;
   _MESSAGE("got string %s", buf);
   g_strData = buf;
   return true;
};
void ScriptCommands::OBSESamples::SerializedData01::OnNewGame(void* reserved) {
   g_strData.clear();
};
void ScriptCommands::OBSESamples::SerializedData01::OnPreload(void* reserved) {
   //
   // TODO: SerializationManager should treat Preload the same way it treats Load
   //
};

bool ScriptCommands::OBSESamples::SerializedData02::Save(SerializationHelper* writer) {
   writer->Write("hello world", 11);
   return true;
};
bool ScriptCommands::OBSESamples::SerializedData02::Load(SerializationHelper* reader, UInt32 version, UInt32 length) {
   g_strData.clear();
   //
   char buf[512];
   reader->ReadBuffer(buf, length);
   buf[length] = 0;
   _MESSAGE("ASDF chunk = %s", buf);
   return true;
};

// ==================================================================================================
//    INFRASTRUCTURE FOR THESE COMMANDS (called "Array API example" in original OBSE plugin source)
// ==================================================================================================

typedef OBSEArrayVarInterface::Array	OBSEArray;
typedef OBSEArrayVarInterface::Element	OBSEElement;

// helper function for creating an OBSE StringMap from a std::map<std::string, OBSEElement>
OBSEArray* StringMapFromStdMap(const std::map<std::string, OBSEElement>& data, Script* callingScript) {
   // create empty string map
   OBSEArray* arr = g_arrayIntfc->CreateStringMap(NULL, NULL, 0, callingScript);

   // add each key-value pair
   for (std::map<std::string, OBSEElement>::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
      g_arrayIntfc->SetElement(arr, iter->first.c_str(), iter->second);
   }

   return arr;
}

// helper function for creating an OBSE Map from a std::map<double, OBSEElement>
OBSEArray* MapFromStdMap(const std::map<double, OBSEElement>& data, Script* callingScript) {
   OBSEArray* arr = g_arrayIntfc->CreateMap(NULL, NULL, 0, callingScript);
   for (std::map<double, OBSEElement>::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
      g_arrayIntfc->SetElement(arr, iter->first, iter->second);
   }

   return arr;
}

// helper function for creating OBSE Array from std::vector<OBSEElement>
OBSEArray* ArrayFromStdVector(const std::vector<OBSEElement>& data, Script* callingScript) {
   OBSEArray* arr = g_arrayIntfc->CreateArray(&data[0], data.size(), callingScript);
   return arr;
}

// ==================================================================================================
//    COMMAND CODE
// ==================================================================================================

#if OBLIVION
   bool Cmd_TestExtractArgsEx_Execute(COMMAND_ARGS) {
	   UInt32 i = 0;
	   char str[0x200] = { 0 };
	   *result = 0.0;

	   if (g_scriptInterface->ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &i, str)) {
		   Console_Print("TestExtractArgsEx >> int: %d str: %s", i, str);
		   *result = 1.0;
	   } else {
		   Console_Print("TestExtractArgsEx >> couldn't extract arguments");
	   }

	   return true;
   }
   bool Cmd_TestExtractFormatString_Execute(COMMAND_ARGS) {
	   char str[0x200] = { 0 };
	   int i = 0;
	   TESForm* form = NULL;
	   *result = 0.0;

	   if (g_scriptInterface->ExtractFormatStringArgs(0, str, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, SIZEOF_FMT_STRING_PARAMS + 2, &i, &form)) {
         Console_Print("TestExtractFormatString >> str: %s int: %d form: %08X", str, i, form ? form->refID : 0);
         *result = 1.0;
	   } else {
         Console_Print("TestExtractFormatString >> couldn't extract arguments.");
	   }

	   return true;
   }
   bool Cmd_ExamplePlugin_0019Additions_Execute(COMMAND_ARGS) {
	   // tests and demonstrates 0019 additions to plugin API
	   // args:
	   //	an array ID as an integer
	   //	a function script with the signature {int, string, refr} returning a string
	   // return:
	   //	an array containing the keys and values of the original array

	   UInt32 arrID = 0;
	   TESForm* funcForm = NULL;

	   if (g_scriptInterface->ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &arrID, &funcForm)) {

		   // look up the array
		    OBSEArray* arr = g_arrayIntfc->LookupArrayByID(arrID);
		    if (arr) {
             //
			    // get contents of array
             //
			    UInt32 size = g_arrayIntfc->GetArraySize(arr);
			    if (size != -1) {
				    OBSEElement* elems = new OBSEElement[size];
				    OBSEElement* keys = new OBSEElement[size];
                //
				    if (g_arrayIntfc->GetElements(arr, elems, keys)) {
					    OBSEArray* newArr = g_arrayIntfc->CreateArray(NULL, 0, scriptObj);
					    for (UInt32 i = 0; i < size; i++) {
						    g_arrayIntfc->SetElement(newArr, i*2, elems[i]);
						    g_arrayIntfc->SetElement(newArr, i*2+1, keys[i]);
					    }
                   //
					    // return the new array
					    g_arrayIntfc->AssignCommandResult(newArr, result);
				    }
				    delete[] elems;
				    delete[] keys;
			    }
		    }
		    if (funcForm) {
			    Script* func = OBLIVION_CAST(funcForm, TESForm, Script);
			    if (func) {
                //
				    // call the function
                //
				    OBSEElement funcResult;
				    if (g_scriptInterface->CallFunction(func, thisObj, NULL, &funcResult, 3, 123456, "a string", *g_thePlayer)) {
					    if (funcResult.GetType() == funcResult.kType_String) {
						    Console_Print("Function script returned string %s", funcResult.String());
					    } else {
						    Console_Print("Function did not return a string");
					    }
				    } else {
					    Console_Print("Could not call function script");
				    }
			    } else {
				    Console_Print("Could not extract function script argument");
			    }
		    }
	   }
	   return true;
   }

   bool Cmd_ExamplePlugin_MakeArray_Execute(COMMAND_ARGS) {
	   // Create an array of the format
	   // { 
	   //	 0:"Zero"
	   //	 1:1.0
	   //	 2:PlayerRef
	   //	 3:StringMap { "A":"a", "B":123.456, "C":"manually set" }
	   //	 4:"Appended"
	   //	}

	   // create the inner StringMap array
	   std::map<std::string, OBSEElement> stringMap;
	   stringMap["A"] = "a";
	   stringMap["B"] = 123.456;

	   // create the outer array
	   std::vector<OBSEElement> vec;
	   vec.push_back("Zero");
	   vec.push_back(1.0);
	   vec.push_back(*g_thePlayer);
	
	   // convert our map to an OBSE StringMap and store in outer array
	   OBSEArray* stringMapArr = StringMapFromStdMap(stringMap, scriptObj);
	   vec.push_back(stringMapArr);

	   // manually set another element in stringmap
	   g_arrayIntfc->SetElement(stringMapArr, "C", "manually set");

	   // convert outer array
	   OBSEArray* arr = ArrayFromStdVector(vec, scriptObj);

	   // append another element to array
	   g_arrayIntfc->AppendElement(arr, "appended");

	   if (!arr)
		   Console_Print("Couldn't create array");

	   // return the array
	   if (!g_arrayIntfc->AssignCommandResult(arr, result))
		   Console_Print("Couldn't assign array to command result.");

	   // result contains the new ArrayID; print it
	   Console_Print("Returned array ID %.0f", *result);

	   return true;
   }

   bool Cmd_PluginTest_Execute(COMMAND_ARGS) {
	   _MESSAGE("plugintest");

	   *result = 42;

	   Console_Print("plugintest running");

	   return true;
   }

   bool Cmd_ExamplePlugin_PrintString_Execute(COMMAND_ARGS) {
	   Console_Print("PrintString: %s", g_strData.c_str());

	   return true;
   }

   bool Cmd_ExamplePlugin_SetString_Execute(COMMAND_ARGS) {
	   char	data[512];

	   if(ExtractArgs(PASS_EXTRACT_ARGS, &data))
	   {
		   g_strData = data;
		   Console_Print("Set string %s in script %08x", data, scriptObj->refID);
	   }

	   ExtractFormattedString(ScriptFormatStringArgs(0, 0, 0, 0), data);
	   return true;
   }
#endif

// ==================================================================================================
//    PARAM AND COMMAND DEFINITIONS
// ==================================================================================================

static CommandInfo kPluginTestCommand =
   {
	   "plugintest",
	   "",
	   0,
	   "test command for obse plugin",
	   0,		// requires parent obj
	   0,		// doesn't have params
	   NULL,	// no param table

	   HANDLER(Cmd_PluginTest_Execute)
   };

static ParamInfo kParams_ExamplePlugin_0019Additions[2] =
   {
	   { "array var", kParamType_Integer, 0 },
	   { "function script", kParamType_InventoryObject, 0 },
   };

DEFINE_COMMAND_PLUGIN(ExamplePlugin_SetString, "sets a string", 0, 1, kParams_OneString)
DEFINE_COMMAND_PLUGIN(ExamplePlugin_PrintString, "prints a string", 0, 0, NULL)
DEFINE_COMMAND_PLUGIN(ExamplePlugin_MakeArray, test, 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(ExamplePlugin_0019Additions, "tests 0019 API", 0, 2, kParams_ExamplePlugin_0019Additions);

static ParamInfo kParams_TestExtractArgsEx[2] =
   {
	   {	"int",		kParamType_Integer,	0	},
	   {	"string",	kParamType_String,	0	},
   };

static ParamInfo kParams_TestExtractFormatString[SIZEOF_FMT_STRING_PARAMS + 2] =
   {
	   FORMAT_STRING_PARAMS,
	   {	"int",		kParamType_Integer,	0	},
	   {	"object",	kParamType_InventoryObject,	0	},
   };

DEFINE_COMMAND_PLUGIN(TestExtractArgsEx, "tests 0020 changes to arg extraction", 0, 2, kParams_TestExtractArgsEx);
DEFINE_COMMAND_PLUGIN(TestExtractFormatString, "tests 0020 changes to format string extraction", 0, 
					  SIZEOF_FMT_STRING_PARAMS+2, kParams_TestExtractFormatString);

// ==================================================================================================
//    REGISTER
// ==================================================================================================

namespace ScriptCommands {
   namespace OBSESamples {
      bool RegisterCommands(const OBSEInterface* obse) {
         //
         // Set ourselves up for serialization.
         //
         auto data01 = std::make_shared<SerializedData01>();
         auto data02 = std::make_shared<SerializedData02>();
         SerializationManager::GetInstance().RegisterRecord(std::dynamic_pointer_cast<SerializableTopLevelRecord>(data01));
         SerializationManager::GetInstance().RegisterRecord(std::dynamic_pointer_cast<SerializableTopLevelRecord>(data02));

         //
         // Set up our commands.
         //
         obse->RegisterCommand(&kPluginTestCommand);

         obse->RegisterCommand(&kCommandInfo_ExamplePlugin_SetString);
         obse->RegisterCommand(&kCommandInfo_ExamplePlugin_PrintString);

         // commands returning array must specify return type; type is optional for other commands
         obse->RegisterTypedCommand(&kCommandInfo_ExamplePlugin_MakeArray, kRetnType_Array);
         obse->RegisterTypedCommand(&kCommandInfo_ExamplePlugin_0019Additions, kRetnType_Array);

         obse->RegisterCommand(&kCommandInfo_TestExtractArgsEx);
         obse->RegisterCommand(&kCommandInfo_TestExtractFormatString);

         return true;
      };
   };
}