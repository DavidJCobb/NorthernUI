#pragma once
#include "obse/PluginAPI.h"

struct OBSESerializationInterface;

extern PluginHandle g_pluginHandle;
extern const UInt32 g_pluginVersion;
#if OBLIVION
   extern OBSESerializationInterface* g_serialization;
   extern OBSEArrayVarInterface*      g_arrayIntfc;
   extern OBSEScriptInterface*        g_scriptInterface;
   extern OBSEStringVarInterface*     g_Str;
#else
#endif

#define DEFINE_SUBROUTINE_EXTERN(retType, name, address, ...) extern retType(*name##)(__VA_ARGS__);
#define DEFINE_SUBROUTINE(retType, name, address, ...) retType(*name##)(__VA_ARGS__) = (retType(*)(__VA_ARGS__))address;
// Creates a function pointer to an assembly subroutine.
// Usage:
//  
// DEFINE_SUBROUTINE(void, UnknownSubroutineDEADBEEF, 0xDEADBEEF, UInt32, UInt32);
// UnknownSubroutineDEADBEEF(0, 1);

extern void SafeMemset(UInt32 destination, UInt8 byte, UInt32 size); // based on OBSE SafeWriteXX // TODO: some patch files still have their own copy; have them share this one instead
