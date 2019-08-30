#include "obse/PluginAPI.h"
#include "obse_common/SafeWrite.cpp"

#if OBLIVION
   #include "obse/GameAPI.h"

   /*	As of 0020, ExtractArgsEx() and ExtractFormatStringArgs() are no longer directly included in plugin builds.
      They are available instead through the OBSEScriptInterface.
      To make it easier to update plugins to account for this, the following can be used.
      It requires that g_scriptInterface is assigned correctly when the plugin is first loaded.
   */
   #define ENABLE_EXTRACT_ARGS_MACROS 0	// #define this as 0 if you prefer not to use this

   #if ENABLE_EXTRACT_ARGS_MACROS
      #define ExtractArgsEx(...) g_scriptInterface->ExtractArgsEx(__VA_ARGS__)
      #define ExtractFormatStringArgs(...) g_scriptInterface->ExtractFormatStringArgs(__VA_ARGS__)
   #endif
#else
   #include "obse_editor/EditorAPI.h"
#endif
#define COBB_SUPPRESS_CRT_CRASH_HANDLER 0 // force CRT error handling to crash ASAP, in the hopes that CobbCrashLogger.dll can grab that

#include "obse/Script.h" // needed for messaging interface
#include <string>

#include "Services/serialization.h"
#include "Services/INISettings.h"
#include "Services/PatchManagement.h"
#include "Services/Translations.h"
#include "Services/UIPrefs.h"
//#include "ScriptCommands/OBSESamples.h"
//#include "ScriptCommands/HUDReticle.h"
#include "ScriptCommands/XInput.h"
#include "Patches/Miscellaneous.h"
#include "Patches/AlchemyMenu.h"
#include "Patches/AudioMenu.h"
#include "Patches/ClassMenu.h"
#include "Patches/CleanUpAfterMenuQue.h"
#include "Patches/ContainerMenu.h"
#include "Patches/DialogMenu.h"
#include "Patches/EffectSettingMenu.h"
#include "Patches/EnchantingAndSigilStoneMenus.h"
#include "Patches/EnhancedMovement.h"
#include "Patches/Exploratory.h" // ====== TEST PATCHES ======
#include "Patches/ExtendedConsoleCommands.h"
#include "Patches/Fonts.h"
#include "Patches/GameplayMenu.h"
#include "Patches/HUDInfoMenu.h"
#include "Patches/HUDMainMenu.h"
#include "Patches/InventoryMenu.h"
#include "Patches/LockPickMenu.h"
#include "Patches/Logging.h"
#include "Patches/MenuTextInputState.h"
#include "Patches/MessageMenu.h"
#include "Patches/NegotiateMenu.h"
#include "Patches/OptionsMenu.h"
#include "Patches/PersuasionMenu.h"
#include "Patches/QuestAddedMenu.h"
#include "Patches/RaceSexMenu.h"
#include "Patches/SavegameSafeDelete.h"
#include "Patches/SaveMenu.h"
#include "Patches/SleepWaitMenu.h"
#include "Patches/SpellMakingMenu.h"
#include "Patches/StatsMenu.h"
#include "Patches/Selectors.h"
#include "Patches/TagIDs/Main.h" //#include "Patches/Traits.h"
#include "Patches/VanillaHUDReticle.h"
#include "Patches/VanillaMapMenu.h"
#include "Patches/VideoMenu.h"
#include "Patches/XXNMenus.h"
#include "Patches/XboxGamepad/Patch.h"
#include "Patches/Compatibility/Dynamic Map/Main.h"
#include "Patches/Compatibility/Dynamic Training Cost/Main.h"
#include "shared.h"

IDebugLog gLog("Data\\OBSE\\Plugins\\NorthernUI.log");

PluginHandle                g_pluginHandle    = kPluginHandle_Invalid;
OBSESerializationInterface* g_serialization   = nullptr;
OBSEArrayVarInterface*      g_arrayIntfc      = nullptr;
OBSEScriptInterface*        g_scriptInterface = nullptr;
const OBSEInterface* g_obse = nullptr;

#define COBB_USING_SERIALIZATION 0

/*************************
   Messaging API example
*************************/

OBSEMessagingInterface* g_msg;

void MessageHandler(OBSEMessagingInterface::Message* msg) {
   switch (msg->type) {
      case OBSEMessagingInterface::kMessage_ExitGame:
         _MESSAGE("MessageHandler received ExitGame message");
         break;
      case OBSEMessagingInterface::kMessage_ExitToMainMenu:
         _MESSAGE("MessageHandler received ExitToMainMenu message");
         break;
      case OBSEMessagingInterface::kMessage_PostLoad:
         _MESSAGE("MessageHandler received PostLoad mesage");
         PatchManager::GetInstance().FireEvent(PatchManager::Req::X_PostLoad);
         break;
      case OBSEMessagingInterface::kMessage_LoadGame:
      case OBSEMessagingInterface::kMessage_SaveGame:
         _MESSAGE("MessageHandler received save/load message with file path %s", msg->data);
         break;
      /*case OBSEMessagingInterface::kMessage_Precompile: // log spam from editor
         {
            ScriptBuffer* buffer = (ScriptBuffer*)msg->data;		
            _MESSAGE("Plugin Example received precompile message. Script Text:\n%s", buffer->scriptText);
            break;
         }*/
      case OBSEMessagingInterface::kMessage_PreLoadGame:
         _MESSAGE("MessageHandler received pre-loadgame message with file path %s", msg->data);
         break;
      case OBSEMessagingInterface::kMessage_ExitGame_Console:
         _MESSAGE("MessageHandler received quit game from console message");
         break;
      default:
         _MESSAGE("MessageHandler received unknown message with type %d", msg->type);
         break;
   }
}

#if COBB_SUPPRESS_CRT_CRASH_HANDLER == 1
   __declspec(naked) void SuppressCRTCrashHandling(
      const wchar_t* expression,
      const wchar_t* function,
      const wchar_t* file,
      unsigned int line,
      uintptr_t pReserved
   ) {
      //
      // All of those parameters are nullptr when we aren't compiled in debug mode,
      // so just force a crash as quickly as possible so our separate crash logger 
      // DLL can catch it and hopefully give us something useful.
      //
      _asm {
         mov eax, 0;
         mov dword ptr[eax], 0;
         retn;
      };
   };
#endif

extern "C" {
   bool OBSEPlugin_Query(const OBSEInterface* obse, PluginInfo* info) {
      _MESSAGE("query");

      // fill out the info structure
      info->infoVersion = PluginInfo::kInfoVersion;
      info->name        = "NorthernUI";
      info->version     = g_pluginVersion; // see shared.cpp

      {  // log our version number -- be helpful!
         auto v = info->version;
         UInt8 major = v >> 0x18;
         UInt8 minor = (v >> 0x10) & 0xFF;
         UInt8 patch = (v >> 0x08) & 0xFF;
         UInt8 build = v & 0xFF;
         _MESSAGE("Version %d.%d.%d, build %d.", major, minor, patch, build);
      }
      {  // Get run-time information
         HMODULE    baseAddr = GetModuleHandle("NorthernUI"); // DLL filename
         MODULEINFO info;
         if (baseAddr && GetModuleInformation(GetCurrentProcess(), baseAddr, &info, sizeof(info)))
            _MESSAGE("We're loaded to the span of memory at %08X - %08X.", info.lpBaseOfDll, (UInt32)info.lpBaseOfDll + info.SizeOfImage);
      }
      if (obse->isEditor)
         _MESSAGE("We've been loaded inside of the Construction Set.");

      // version checks
      if(!obse->isEditor) {
         if(obse->obseVersion < OBSE_VERSION_INTEGER && obse->obseVersion < 21) {
            _ERROR("OBSE version too old (got %08X expected at least %08X)", obse->obseVersion, OBSE_VERSION_INTEGER);
            return false;
         }
         #if OBLIVION
            if(obse->oblivionVersion != OBLIVION_VERSION) {
               _ERROR("incorrect Oblivion version (got %08X need %08X)", obse->oblivionVersion, OBLIVION_VERSION);
               return false;
            }
         #endif
         //
         g_serialization = (OBSESerializationInterface *)obse->QueryInterface(kInterface_Serialization);
         if(!g_serialization) {
            _ERROR("serialization interface not found");
            return false;
         }

         if(g_serialization->version < OBSESerializationInterface::kVersion) {
            _ERROR("incorrect serialization version found (got %08X need %08X)", g_serialization->version, OBSESerializationInterface::kVersion);
            return false;
         }

         g_arrayIntfc = (OBSEArrayVarInterface*)obse->QueryInterface(kInterface_ArrayVar);
         if (!g_arrayIntfc) {
            _ERROR("Array interface not found");
            return false;
         }

         g_scriptInterface = (OBSEScriptInterface*)obse->QueryInterface(kInterface_Script);
      } else {
         // no version checks needed for editor
      }
      // version checks pass
      return true;
   }

   bool OBSEPlugin_Load(const OBSEInterface* obse) { 
      //
      // Plugins load before the game does anything substantial, so most game objects 
      // and singletons will be under construction or non-existent when this runs.
      //
      _MESSAGE("load");

      #if COBB_SUPPRESS_CRT_CRASH_HANDLER == 1
         _set_invalid_parameter_handler(SuppressCRTCrashHandling);
      #endif

      g_obse = obse;

      g_pluginHandle = obse->GetPluginHandle();

      /***************************************************************************
       *	
       *	READ THIS!
       *	
       *	Before releasing your plugin, you need to request an opcode range from
       *	the OBSE team and set it in your first SetOpcodeBase call. If you do not
       *	do this, your plugin will create major compatibility issues with other
       *	plugins, and may not load in future versions of OBSE. See
       *	obse_readme.txt for more information.
       *	
       **************************************************************************/
      // register commands
      //obse->SetOpcodeBase(0x2000);
      //ScriptCommands::HUDReticle::RegisterCommands(obse);
      //ScriptCommands::OBSESamples::RegisterCommands(obse);


      // set up serialization callbacks when running in the runtime
      if (!obse->isEditor) {
         #if COBB_USING_SERIALIZATION == 1
            //
            // NOTE: SERIALIZATION DOES NOT WORK USING THE DEFAULT OPCODE BASE IN RELEASE BUILDS OF OBSE.
            //
            SerializationManager::GetInstance().Init(g_pluginHandle, g_serialization); // Cobb singleton
         #endif
         {  // Register for string var interface.
            // This allows plugin commands to support '%z' format specifier in format string arguments.
            OBSEStringVarInterface* g_Str = (OBSEStringVarInterface*) obse->QueryInterface(kInterface_StringVar);
            g_Str->Register(g_Str);
         }
         {  // Get an OBSEScriptInterface to use for argument extraction.
            g_scriptInterface = (OBSEScriptInterface*) obse->QueryInterface(kInterface_Script);
         }
         (NorthernUI::INI::INISettingManager::GetInstance()).Load(); // load and cache INI settings from a file (NOTE: must be done before patches are applied)
         {
            auto& man = PatchManager::GetInstance();
            man.Expect(42);
            man.RegisterPatch("Miscellaneous", &CobbPatches::Miscellaneous::Apply, {});
            man.RegisterPatch("Exploratory",   &CobbPatches::Exploratory::Apply,   {});
            man.RegisterPatch("Logging",       &CobbPatches::Logging::Apply,       {});
            man.RegisterPatch("EnhancedMovement",   &CobbPatches::EnhancedMovement::Apply,   {});
            man.RegisterPatch("MenuTextInputState", &CobbPatches::MenuTextInputState::Apply, {});
            man.RegisterPatch("SavegameSafeDelete", &CobbPatches::SavegameSafeDelete::Apply, {});
            man.RegisterPatch(
               "SavegameSafeDelete::DeleteToRecycleBin",
               &CobbPatches::SavegameSafeDelete::DeleteToRecycleBin::DelayedApply,
               { PatchManager::Req::G_MainMenu }
            );
            man.RegisterPatch("Selectors",     &CobbPatches::Selectors::Apply, {});
            man.RegisterPatch("TagIDs",        &CobbPatches::TagIDs::Apply,    { PatchManager::Req::P_MenuQue });
            man.RegisterPatch("Fonts",         &CobbPatches::Fonts::Apply,     {});
            man.RegisterPatch("XXNMenus",      &CobbPatches::XXNMenus::Apply,  { PatchManager::Req::G_FileFinder });
            man.RegisterPatch("XboxGamepad",   &CobbPatches::XboxGamepad::Apply, {});
            //
            man.RegisterPatch("CleanUpAfterMenuQue", &CobbPatches::CleanUpAfterMenuQue::Apply, { PatchManager::Req::X_PostLoad });
            //
            // vanilla menu patches:
            //
            man.RegisterPatch("AlchemyMenu",    &CobbPatches::AlchemyMenu::Apply, {});
            man.RegisterPatch("AudioMenu",      &CobbPatches::AudioMenu::Apply, {});
            man.RegisterPatch("ClassMenu",      &CobbPatches::ClassMenu::Apply, {});
            man.RegisterPatch("DialogMenu",     &CobbPatches::DialogMenu::Apply, {});
            man.RegisterPatch("ContainerMenu",  &CobbPatches::ContainerMenu::Apply, {});
            man.RegisterPatch("EffectSettingMenu", &CobbPatches::EffectSettingMenu::Apply, {});
            man.RegisterPatch("EnchantingAndSigilStoneMenus", &CobbPatches::EnchantingAndSigilStoneMenus::Apply, {});
            man.RegisterPatch("ExtendedConsoleCommands", &CobbPatches::ExtendedConsoleCommands::Apply, {});
            man.RegisterPatch("GameplayMenu",   &CobbPatches::GameplayMenu::Apply, {});
            man.RegisterPatch("HUDInfoMenu",    &CobbPatches::HUDInfoMenu::Apply, {});
            man.RegisterPatch("HUDMainMenu",    &CobbPatches::HUDMainMenu::Apply, {});
            man.RegisterPatch("HUDReticle",     &CobbPatches::HUDReticle::Apply, {});
            man.RegisterPatch("InventoryMenu",  &CobbPatches::InventoryMenu::Apply, {});
            man.RegisterPatch("LockPickMenu",   &CobbPatches::LockPickMenu::Apply, {});
            man.RegisterPatch("MapMenu",        &CobbPatches::MapMenu::Apply, { PatchManager::Req::P_XInput });
            man.RegisterPatch("MessageMenu",    &CobbPatches::MessageMenu::Apply, {});
            man.RegisterPatch("NegotiateMenu",  &CobbPatches::NegotiateMenu::Apply, {});
            man.RegisterPatch("OptionsMenu",    &CobbPatches::OptionsMenu::Apply, {});
            man.RegisterPatch("PersuasionMenu", &CobbPatches::PersuasionMenu::Apply, {});
            man.RegisterPatch("QuestAddedMenu", &CobbPatches::QuestAddedMenu::Apply, {});
            man.RegisterPatch("RaceSexMenu",    &CobbPatches::RaceSexMenu::Apply, {});
            man.RegisterPatch("SaveMenu",       &CobbPatches::SaveMenu::Apply, {});
            man.RegisterPatch("SleepWaitMenu",  &CobbPatches::SleepWaitMenu::Apply, {});
            man.RegisterPatch("SpellMakingMenu", &CobbPatches::SpellMakingMenu::Apply, {});
            man.RegisterPatch("StatsMenu",      &CobbPatches::StatsMenu::Apply, {});
            man.RegisterPatch("VideoMenu",      &CobbPatches::VideoMenu::Apply, {});
            //
            // compat:
            //
            man.RegisterPatch("DynamicMapEmulation", &CobbPatches::DynamicMapEmulation::Apply, { PatchManager::Req::G_MainMenu });
            man.RegisterPatch("DynamicTrainingCost", &CobbPatches::DynamicTrainingCost::Apply, { PatchManager::Req::G_MainMenu });
            //
            // services that need to init after certain patches have run:
            //
            PatchManager::GetInstance().RegisterPatch("UIPrefManager:Load", &UIPrefManager::initialize, { PatchManager::Req::P_Selectors });
         }

         // register to receive messages from OBSE
         OBSEMessagingInterface* msgIntfc = (OBSEMessagingInterface*)obse->QueryInterface(kInterface_Messaging);
         msgIntfc->RegisterListener(g_pluginHandle, "OBSE", MessageHandler);
         g_msg = msgIntfc;

         (NorthernUI::L10N::StrManager::GetInstance()).Update(); // load translatable strings from a file
      }
      obse->SetOpcodeBase(0x28D0); // We may use 0x28D0 to 0x28DF, inclusive
      {  // Register script commands.
         obse->RegisterCommand(&kCommandInfo_IsGamepadKeyPressed); // 28D0
         obse->RegisterCommand(&kCommandInfo_DisableGamepadKey); // 28D1
         obse->RegisterCommand(&kCommandInfo_EnableGamepadKey); // 28D2
         obse->RegisterCommand(&kCommandInfo_IsGamepadKeyDisabled); // 28D3
         obse->RegisterCommand(&kCommandInfo_GetGamepadControl); // 28D4
         obse->RegisterCommand(&kCommandInfo_IsGamepadConnected); // 28D5
         obse->RegisterCommand(&kCommandInfo_GetGamepadJoystickMagnitude); // 28D6
         obse->RegisterCommand(&kCommandInfo_GetGamepadTriggerMagnitude); // 28D7
         obse->RegisterCommand(&kCommandInfo_IsGamepadKeyPressed); // 28D8
         obse->RegisterCommand(&kCommandInfo_IsGamepadKeyPressed); // 28D9
         obse->RegisterCommand(&kCommandInfo_IsGamepadKeyPressed); // 28DA
      }
      /*//
      // get command table, if needed
      OBSECommandTableInterface* cmdIntfc = (OBSECommandTableInterface*)obse->QueryInterface(kInterface_CommandTable);
      if (cmdIntfc) {
         #if 0	// enable the following for loads of log output
         for (const CommandInfo* cur = cmdIntfc->Start(); cur != cmdIntfc->End(); ++cur) {
            _MESSAGE("%s",cur->longName);
         }
         #endif
      } else {
         _MESSAGE("Couldn't read command table");
      }
      //*/
      return true;
   }
};
